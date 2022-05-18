#include <iostream>
#include <pthread.h>
#include <unistd.h>

#define READ_THREADS 9

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

int a[1000];

// add read-write lock
void *read(void *) {
  while (true) {
    int sum = 0;

    //
    pthread_rwlock_rdlock(&rwlock);

    for (int i = 0; i < 1000; i++) {
      sum += a[i];
    }

    if (sum % 1000 != 0) {
      std::cerr << "failed" << std::endl;
      // std::cerr << "failed\n";
      exit(-1);
    } else if (sum >= 100000) {
      std::cerr << "success" << std::endl;
      // std::cerr << "success\n";
      exit(0);
    }

    //
    pthread_rwlock_unlock(&rwlock);

    usleep(1);
  }
}

void *write(void *) {
  while (true) {

    //
    pthread_rwlock_wrlock(&rwlock);

    for (int i = 0; i < 1000; i++) {
      a[i] += 1;
    }

    //
    pthread_rwlock_unlock(&rwlock);

    usleep(2);
  }
}

int main() {

  int i;
  pthread_t r_threads[READ_THREADS], w_thread;

  // pthread_rwlock_init(&rwlock, NULL);

  // pthread_create
  pthread_create(&w_thread, NULL, write, NULL);

  for (i = 0; i < READ_THREADS; i++) {
    pthread_create(&r_threads[i], NULL, read, NULL);
  }

  // pthread_join
  pthread_join(w_thread, NULL);

  for (i = 0; i < READ_THREADS; i++) {
    pthread_join(r_threads[i], NULL);
  }

  return 0;
}

// g++ -std=c++11 -o hw6 hw6.cpp -lpthread
