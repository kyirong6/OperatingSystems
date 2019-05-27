#inclde "list.h"
#include <stdlib.h>

typedef struct nodeStruct {
    int item;
    struct nodeStruct *next;
} Node;


Node* List_createNode(int item) {
  Node* node;
  node = (Node*) malloc(sizeof(Node));

  node->item = item;
  node->next = NULL;
  return node;
}

void List_insertHead(Node **headRef, Node node) {
  node->next = (*headRef);
  (*headRef) = node;
}

void List_insertTail(Node **headRef, Node node) {
  

}



