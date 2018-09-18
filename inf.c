#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/** A helper method that acts as a background process. 
	Makes its presence known by printing out a tag at an interval (in seconds). **/
int main(int argc, char* argv[]) {
	if (argc != 3) {
		printf("Error: not enough args. Please specify a tag and an interval (in that order)\n");
		exit (1);
	}
		
	char* tag = argv[1];
	int interval = atoi(argv[2]);
		
	while (1) {
		printf("%s\n", tag);
		sleep(interval);
	}
	
	exit (0);
}