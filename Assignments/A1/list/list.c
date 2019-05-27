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
  node->next = *headRef;
  *headRef = node;
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


int List_countNodes(Node *head) {
  if (head == NULL) {
    return 0;
  }

  int count = 1;
  Node* curr = head;
  while (curr->next != NULL) {
    curr = curr->next;
    count++;
  }
  
  return count;
}


Node* List_findNode(Node* head, int item) {
  if (head == NULL) {
    return NULL;
  }

  Node* curr = head;
  while ((curr->item != item) && (curr->next != NULL)) {
    curr = curr->next;
  }

  if (curr->item == item){
    return curr;
  } else {
    return NULL;
  }
}


void List_deleteNode(Node **headRef, Node* node) {
  Node* head = *headRef;
  
  if (head->next == NULL) {
    *headRef = NULL;
    free(head);
    return;
  }

  if (head->item == node->item) {
    *headRef = head->next;
    free(head);
    return;
  }

  while (head->next->item != node->item) {
    head = head->next;
  }

  Node* temp = head->next;
  head->next = temp->next;
  free(temp);
  return;
}


void List_sort(Node **headRef) {
  



}

