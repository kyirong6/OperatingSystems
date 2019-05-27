// Sample test routine for the list module.
#include <stdio.h>
#include "list.h"
#include <stdbool.h>
#include <string.h>
#include <assert.h>

/*
 * Main()
 */
int main(int argc, char** argv)
{
	printf("Starting tests...\n");
	Node* head = NULL;
	/*
	Node* head2 = NULL;

	Node* var = List_createNode(5);
	Node* var2 = List_createNode(6);
	Node* var3 = List_createNode(7);
	List_insertHead(&head, var);
	List_insertTail(&head, var2);
	List_insertTail(&head, var3);
	printf("%i", head->item);
	printf("%i", head->next->item);
	printf("%i", head->next->next->item);
	printf("%i", List_countNodes(head));
	printf("%i", List_countNodes(head2));
	*/
	
       
	
	// Starting count:
	assert(List_countNodes(head) == 0);

	// Create 1 node:
	struct nodeStruct* firstNode = List_createNode(0);
	List_insertHead(&head, firstNode);
	assert(List_countNodes(head) == 1);
	assert(List_findNode(head, 0) == firstNode);
	assert(List_findNode(head, 1) == NULL);

	// Insert tail:
	struct nodeStruct* lastNode = List_createNode(-5);
	List_insertTail(&head, lastNode);
	assert(List_countNodes(head) == 2);
	assert(List_findNode(head, 0) == firstNode);
	assert(List_findNode(head, -5) == lastNode);
	assert(List_findNode(head, 1) == NULL);

	// Verify list:
	struct nodeStruct *current = head;
	assert(current->item == 0);
	assert(current->next != NULL);
	current = current->next;
	assert(current->item == -5);
	assert(current->next == NULL);

	/*
	// Sort and verify:
	List_sort(&head);
	current = head;
	assert(current->item == -5);
	assert(current->next != NULL);
	current = current->next;
	assert(current->item == 0);
	assert(current->next == NULL);
	*/
	
	// Delete
	assert(List_countNodes(head) == 2);
	struct nodeStruct *nodeOf0 = List_findNode(head, 0);
	List_deleteNode(&head, nodeOf0);
	assert(List_countNodes(head) == 1);
	assert(List_findNode(head, 0) == NULL);
	assert(List_findNode(head, 1) == NULL);
	current = head;
	assert(current->item == -5);
	assert(current->next == NULL);
	

	printf("\nExecution finished.\n");
	return 0;
}
