#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <map>

#include <cstring>
#include <sys/mman.h>

using namespace std;

int main(int argc, char *argv[]) {
  ifstream fin(argv[1]);

  int N = atoi(argv[2]);

  int nlines = 0;

  int x, j;

  while (fin >> x) {
    ++nlines;
  }

  // number of lines that each process needs to process
  int nn = (nlines / N) % 2 == 0 ? nlines / N : nlines / N + 1;

  fin.close();

  fin.open(argv[1]);

  // read in the file sequentially in the main process
  // store a temp file of input for each child process
  for (int i = 0; i < N; ++i) {
    ofstream fout("p" + to_string(i));
    j = 0;
    while (j < nn && fin >> x) {
      fout << x << endl;
      ++j;
    }
    fout.close();
  }
  fin.close();

  int i = 0;
  int nprocs = 0;

  map<int, int> counts;

  // mmap fields
  size_t length;
  int prot;
  int flags;
  // int fd;
  off_t offset;

  length = 10485760; // 10485760 bytes * N children * sizeof(int)
  prot = PROT_READ | PROT_WRITE;
  flags = MAP_SHARED | MAP_ANONYMOUS; // shared between parent/child processes?

  /*
  char ptmp[] = "/tmp/ptmpXXXXXX";
  fd = mkstemp(ptmp);            // temp file fd
  ftruncate(fd, length * N + 1); // truncate temp to non-zero length
  */

  offset = 0;

  int *shared =
      (int *)mmap(0, length * N * sizeof(int), prot, flags, -1, offset);

  while (nprocs < N) {
    // call fork in the loop
    pid_t rc = fork();

    if (rc < 0) {
      exit(1);
    } else if (rc == 0) { // child (new process)

      // each child process reads in its input and count the numbers

      // read from temp files
      ifstream fin("p" + to_string(nprocs));

      offset = length * nprocs; // child offset (in bytes) within shared region

      for (int i = 0; i < length; i++) {
        shared[offset + i] = -1;
      }

      int size = 0; // actual amount of numbers written to a shared region
      for (int i = 0; fin >> x; ++i) { // write to shared region
        shared[offset + i] = x;
        ++size;
      }
      fin.close();

      /*
      while (fin >> x) {
        if (counts.find(x) == counts.end())
          counts[x] = 0;
        ++counts[x];
      }
      */

      remove(("p" + to_string(nprocs)).c_str()); // don't need again

      for (int i = 0; i < size; ++i) {
        int next = shared[offset + i];
        shared[offset + i] = -1; // reset

        if (counts.find(next) == counts.end())
          counts[next] = 0;
        ++counts[next];
      }

      /*
      for (int i = 0; shared[offset + i] != 0; i++) {
        shared[offset + i] = 0;
      }

      // write to file
      ofstream fout("cp" + to_string(nprocs));
      for (auto &c : counts) {
        fout << c.first << " " << c.second << endl;
      }
      fout.close();
      */

      // write to shared region
      int i = 0;
      for (auto &c : counts) { // alternating (number, count) pairs
        shared[offset + i] = c.first;
        shared[offset + i + 1] = c.second;
        i += 2;
      }

      return 0;
    } else { // parent goes down this path (main)
      // wait function should not be invoked here
      ++nprocs;
    }
  }

  // wait function can only be invoked after all processes are created
  // call wait or waitpid for nprocs times to make sure all child processes
  // have returned
  while (nprocs > 0) {
    wait(NULL);
    --nprocs;
  }

  /*
  // debug -- print shared regions
  for (int proc = 0; proc < N; ++proc) {
    for (int i = 0; i < length; i++) {
      int curr = shared[length * proc + i];
      if (curr == -1) {
        break;
      }
      printf("R%d: %d\n", proc + 1, curr);
    }
  }
  */

  /*
  // the main process merges the results for child processes
  for (int i = 0; i < N; ++i) {
    fin.open("cp" + to_string(i));
    while (fin >> x >> j) {
      if (counts.find(x) == counts.end())
        counts[x] = 0;
      counts[x] += j;
    }
    fin.close();
    remove(("cp" + to_string(i)).c_str());
  }
  for (auto &c : counts) {
    cout << c.first << " " << c.second << endl;
  }
  */

  // the main process merges the results for shared regions
  for (int i = 0; i < N; ++i) {
    offset = length * i;

    int it = 0;
    while (shared[offset + it] != -1) {
      x = shared[offset + it];
      j = shared[offset + it + 1];
      if (counts.find(x) == counts.end()) {
        counts[x] = 0;
      }
      counts[x] += j;
      it += 2; // leap frog
    }
  }
  for (auto &c : counts) {
    cout << c.first << " " << c.second << endl;
  }

  // sleep(3);

  // tidy up
  munmap(shared, length * N);
  // close(fd);
  // unlink(ptmp);

  return 0;
}
