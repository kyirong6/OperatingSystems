#include "list.h"
#include <stdlib.h>

Node* List_createNode(int item) {
  Node* node;
  node = (Node*) malloc(sizeof(Node));

  node->item = item;
  node->next = NULL;
  return node;
}

void List_insertHead(Node **headRef, Node* node) {
  node->next = (*headRef);
  (*headRef) = node;
  return;
}

void List_insertTail(Node **headRef, Node* node) {
  node->next = NULL;

  if (*headRef == NULL) {
    *headRef = node;
    return;
  }
  
  Node* curr =  *headRef;
  while (curr->next != NULL) {
    curr = curr->next;
  }
  curr->next = node;

  return;
}




