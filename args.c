#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/** A helper method that prints out all args passed to it. **/
int main(int argc, char* argv[]) {
	
	int i = 0;
	for (i = 0; i < args; i++) {
		printf("\n%s", argv[i]);
	}
	
	exit (0);
}