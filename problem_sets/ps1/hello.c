#include <stdio.h>
#include <stdlib.h>


void say_hello (int times) {
    for (int i=0; i < times; i++) {
      printf ("Hello World\n");
    }
}



int main (int argc, char *argv[]) {
    say_hello(atoi(argv[1]));
return 0; }
