#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// a helper method that prints out a given tag at a given interval
int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: inf tag interval\n");
	} else {
		const char* tag = argv[1];
		int interval = atoi(argv[2]);
		while(1) {
			printf("%s\n", tag);
			sleep(interval);
		}
	}
}