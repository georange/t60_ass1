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
//	char* name;
    struct node* next;
};

// global variables to keep track of head of queue, and queue size
struct node* queue_head = NULL;
int size = 0;

// global variable for list of accepted commands
char* commands[] = {"bg", "bglist", "bgkill", "bgstop", "bgstart", "pstat"};


// inserts a process node to the end of the queue
void insert(pid_t pid) {
    struct node *curr = queue_head;
    while (curr->next != NULL) {
        curr = curr->next;
    }

    curr->next = malloc(sizeof(struct node));
	if (!curr->next) {
		prinf("Error: unsuccessful making new node.\n");
		return;
	}
	
    curr->next->pid = pid;
    curr->next->next = NULL;
	size++;
}

// deltes a process node from anywhere in the queue by pid
void delete(pid_t pid) {
	struct node *curr = queue_head;
	struct node *prev = NULL;

	while (curr != NULL) {
		if (curr->pid == pid) {
			if (curr == queue_head) {
				queue_head = queue_head->next;
			} else {
				prev->next = curr->next;
			}
			free(curr);
			size--;
			return;
		}
		prev = curr;
		curr = curr->next;
	}
	
	prinf("Error: process not found.\n");
	return;
}

// figures out which command was issued, returns -1 if not on the list of accepted commands
int get_command (char* command) {
	if (command) {
		int i;
		for (i = 0; i < MAX_COMMAND; i++) {
			if (!strcmp(command, commands[i])) {
				return i;
			}
		}
	}
	
	printf("Error: invalid command. Please enter one of these: bg, bglist, bgkill, bgstop, bgstart, pstat.\n");
	return -1;
} 

// parses for a valid number to act as pid, returns -1 if not a valid number
int parse_pid (char* pid) {
	if (pid) {
		int val = atoi(pid);
		if (val) {
			return val;
		}
	}
	
	printf("Error: invalid pid. Please enter an integer.\n");
	return -1;
}

// parses user input and runs command if possible
int run_input (char copy[]) {
	char* tok;
	tok = strtok (copy, " "); 
	
	// match command to a valid command
	int command = get_command(tok);
			
	// command is bg
	if (command == 0) {
		char* program = strtok(NULL," ");
		if (!program) {
			printf("Error: arg needed. Please enter the program you wish to run.\n");
		} else {
		
			// HERE
		}
		
	// command is bglist	
	} else if (command == 1) {
		char* not_empty = strtok(NULL," ");
		if (not_empty) {
			printf("Error: arg not needed for this command. Please try again.\n");
		} else {
			
			// HERE
		}
		
	// command is bgkill, bgstop, bgstart, or pstat 
	} else if (command > 1) {
		// parse for a valid pid
		pid_t target_pid = parse_pid(strtok(NULL," "));
		
		//printf("%d\n", target_pid);
	
		if (target_pid > -1){
		// run command with user inputed command and pid
					
			// HERE
		
		}
	}
}

int main(){
	while(1) {
		char *input = NULL;
		char *prompt = "PMan: > ";

		input = readline(prompt);
		
		// if input give, parse and run if possible
		if (input) {
			// make a copy of input for tokenizing 
			char copy[MAX_INPUT];
			strncpy (copy, input, MAX_INPUT);
			run_input(copy);
			
		}
		
		// check for status updates
		
		// HERE

		//printf("%s\n", input);
	}
	
    exit (0);
}