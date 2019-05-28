Ngawang Kyirong: 301312227
Jong Joon Lee: 301346516

mystring:
  * mystrlen(): strings are stored as arrays of characters so we need to iterate over the array and keep track of our loops (int) until the terminating character is reached.
  
  * mystrcpy(): similar concept as above but the destination array is filled with the contents of the source array instead of keeping track of our loops. Again, the loop ends after reaching the terminating character. Moreover, adding the terminating character to the end of the destination is now needed.
  
  * mystrcmp(): Many ways to implement this one. This particular implementation was used for simplicity. Similar concepts taken from above but it loops until the 2 characters from each string are not equal. If one of the characters is the terminating character, we know that we've reached the end of both string so we return 0 since they must be equal. If not, we take their unsigned char values and subtract them. If the value is negative, the first string must be less than the second so we return a -1. Returning 1 otherwise.
  
  * mystrdup(): Similar concept as above but we must allocate memory for the new char in order to copy it over. This is done using malloc and since we have already defined a mystrlen() function, we can use that as well. We need the +1 to account for the terminating character. The rest is the same principles used before.


list:
  * typedef in list.h: this is done so we can use a more convenient notation instead of struct... all the time.
  
  * List_createNode(): We must allocate memory for the struct pointer again. Since it is just a node on its own, all we need to do is set the item and 'next' will be null.

  * List_insertHead(): Set the new node to the current head and set the head now to the new node. Even if the head is NULL this will work.

  * List_insertTail(): if head is null, then the linkedlist is empty and the tail==head. If not, we need to iterate until we reach the last node (next is null) and set the next node to the new node.

  * List_countNodes(): Similar concept to inserting except we keep track of our loops and exit when we've reached the end.

  * List_findNode(): Initially, we need to check if the list is empty and if so, we return NULL. If not, we loop until we've reached the item we want or the next node is empty. Now, there are 2 cases to check for. We've either reached the wanted node or it doesnt exist. Check for one and return results.

  * List_deleteNode(): Firstly, we check if it is only a single item list. If it is, we set the head to null. Secondly, We check if head is the iteam in a multi item list. If it is, we remove the first node and reset the head to the next item. Lastly, the node is somewhere else in the list so we iterate over the list checking the item ahead of the current node so we can keep track of the current node. Since we are guaranteed the node is inside the list, we dont need to check for NULL. Once we reach the node behind the one we want, we re-arrange the list accordingly by adjusting our 'next' pointer and freeing the node we dont want from memory.

  * List_sort(): Decided to use a bubble sort as it is easy to implement. Also decided to change the values of the node rather than moving the nodes themselves. 
