#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>         
#include <sys/wait.h>          
#include <signal.h> 

#include <readline/readline.h>
#include <readline/history.h>

#define MAX_INPUT 200
#define MAX_COMMAND 6

// node struct to keep track of each process
typedef struct node {
    pid_t pid;
    struct node* next;
};

// global variable to keep track of head of queue
struct node* queue_head = NULL;

// global variable for list of accepted commands
char* commands[] = {"bg", "bglist", "bgkill", "bgstop", "bgstart", "pstat"};


// figures out which command was issued, returns -1 if not on the list of accepted commands
int get_command (char* command) {
	int i;
	for (i = 0; i < MAX_COMMAND; i++) {
		if (!strcmp(command, commands[i])) {
			return i;
		}
	}
	
	printf("Error: invalid command. Please enter one of these: bg, bglist, bgkill, bgstop, bgstart, pstat.\n");
	return -1;
} 

// parses for a valid number to act as pid, returns -1 if not a valid number
int parse_pid (char* pid) {
	int val = atoi(pid);
	if (val) {
		return val;
	}
	
	printf("Error: invalid pid. Please enter an integer.\n");
	return -1;
}

int main(){
	while(1) {
		char *input = NULL;
		char *prompt = "PMan: > ";

		input = readline(prompt);
		
		// tokenize user input for further parsing
		if (input) {
			/* make a copy of input and tokenize it */
			char copy[MAX_INPUT];
			char* tok;
			strncpy (copy, input, MAX_INPUT);
			tok = strtok (copy, " "); 	
			
			int command = get_command(tok);

			if (command > -1) {
				int target_pid = parse_pid(strtok(NULL," "));
				printf ("%d",target_pid);
				if (target_pid > 0){
					printf("in");
					
					// RUN COMMAND WITH COMMAND AND TARGET_PID
					
				} else {
					printf("Error: invalid pid. Please enter an integer.\n");
				}
			}
		}

		//printf("%s\n", input);
	}
	
    exit (0);
}
