#include "mystring.h"
#include <assert.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * The following are simple tests to help you.
 * You should develop more extensive tests to try and find bugs.
 */
int main()
{
        /*
	 * Tests for mystrlen()
	 */
        const int SIZE = 100;
	char buffer[SIZE];

	assert(mystrlen("cmpt 300") == 8);
	assert(mystrlen("") == 0);

	
	/*
	 * Tests for mystrcpy() and mystrcmp()
	 */ 
	const char *varStr = "hello world!";
	char check[100];

	mystrcpy(check, varStr);
	assert(*check == *varStr);

	mystrcpy(buffer, "I am testing my string functions!");
        assert(strcmp(buffer, "I am testing my string functions!") == 0);
	assert(mystrcmp ("I love coding", "I love coding") == 0);
	assert(mystrcmp("I love coding", "I love codinG") == 1);
	assert(mystrcmp("", "") == 0);
	assert(mystrcmp("","\0") == 0);

	
	/*

	char *dupStr = mystrdup(buffer);
	assert (!strcmp(buffer, dupStr));
	assert (buffer != dupStr);

	if (dupStr != NULL)
		free (dupStr);
	*/
	
	printf ("\nPassed VERY simple tests, remember to develop more tests.\n");

	return 0;
}

