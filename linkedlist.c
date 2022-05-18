#include <stdio.h>
#include <stdlib.h>

struct list_node {
  int val;
  struct list_node *next;
};

struct list_type {
  struct list_node *head;
};

void print_list(struct list_type *l) {
  struct list_node *p = l->head;
  while (p) {
    printf("%d ", p->val);
    fflush(stdout);
    p = p->next;
  }
  printf("\n");
  fflush(stdout);
}

struct list_type *new_list() {
  // creat a new list

  struct list_type *head = NULL;
  head = (struct list_type *)malloc(sizeof(struct list_type) * 1);

  return head;
}

void add_value(struct list_type *l, int v) {
  // add a node with value v at the end of list l

  struct list_node *node = NULL;
  struct list_node *curr = NULL;
  node = (struct list_node *)malloc(sizeof(struct list_node) * 1);
  node->val = v;
  node->next = NULL;

  // if head is NULL then list is empty, new node becomes head
  if (!l->head) {
    l->head = node;
  } else {
    curr = l->head;

    // while there is a next node, iterate through nodes
    while (curr->next) {
      curr = curr->next;
    }
    curr->next = node;
  }
}

void insert_value(struct list_type *l, unsigned i, int v) {
  // insert a new node with value v as the ith node of list l

  struct list_node *node = NULL;
  struct list_node *curr = NULL;
  node = (struct list_node *)malloc(sizeof(struct list_node) * 1);
  node->val = v;
  node->next = NULL;

  // insert at beginning of list, update head pointer
  if (i == 0) {
    node->next = l->head;
    l->head = node;
  } else {
    curr = l->head;

    // new node's next points to ith - 1 (prev) node's next
    // prev points to new node
    for (int j = 0; j < i - 1; j++) {
      if (curr->next) {
        curr = curr->next;
      }
    }
    node->next = curr->next;
    curr->next = node;
  }
}

void remove_node(struct list_type *l, unsigned i) {
  // remove the ith node in the list l

  struct list_node *curr = NULL;
  struct list_node *temp = NULL;

  // head node becomes current head's next node
  if (i == 0) {
    curr = l->head;
    l->head = l->head->next;
    free(curr);
  } else {
    curr = l->head;

    for (int j = 0; j < i; j++) {
      if (curr->next) {
        temp = curr;
        curr = curr->next;
      }
    }
    // prev next -> curr next
    temp->next = curr->next;
    free(curr);
  }
}

int main() {
  struct list_type *l = new_list();
  // mine
  //   add_value(l, 0);
  //   add_value(l, 1);
  //   add_value(l, 2);
  //   add_value(l, 3);
  //   add_value(l, 4);
  //   print_list(l);

  //   remove_node(l, 0);
  //   remove_node(l, 3);
  //   insert_value(l, 0, 99);
  //   insert_value(l, 4, 99);
  //   print_list(l);

  add_value(l, 0);
  add_value(l, 1);
  insert_value(l, 0, 4);

  remove_node(l, 0);

  print_list(l); // "0 1"

  add_value(l, 2);

  insert_value(l, 1, 10);
  print_list(l); // "0 10 1 2"

  remove_node(l, 1);
  print_list(l); // "0 1 2"
  return 0;
}