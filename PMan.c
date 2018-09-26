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
	char* name;
    struct node* next;
}node;


/** Global Variables **/

// to keep track of head of queue
struct node* queue_head = NULL;

// list of accepted commands
char* commands[] = {"bg", "bglist", "bgkill", "bgstop", "bgstart", "pstat"};


/** List Functions **/

// inserts a process node to the end of the queue
void insert(pid_t pid, char* name) {
	if (!queue_head) {
		queue_head = (struct node*)malloc(sizeof(struct node));
		queue_head->next = (struct node*)malloc(sizeof(struct node));
		
		queue_head->next->pid = pid;
		queue_head->next->name = name;
		queue_head->next->next = NULL;
		
		printf("%s\n",queue_head->next->name);
		
	} else {
		struct node *curr = queue_head;
		while (curr->next != NULL) {
			curr = curr->next;
		}

		curr->next = (struct node*)malloc(sizeof(struct node));
		curr->next->pid = pid;
		curr->next->name = name;
		curr->next->next = NULL;
		
		printf("%s\n",curr->next->name);
	}
}

// deletes a process node from anywhere in the queue by pid
void delete(pid_t pid) {
	struct node* curr = queue_head;
	struct node* prev = NULL;

	while (curr != NULL) {
		if (curr->pid == pid) {
			if (curr == queue_head) {
				queue_head = queue_head->next;
			} else {
				prev->next = curr->next;
			}
			free(curr);
			return;
		}
		prev = curr;
		curr = curr->next;
	}
	
	printf("Error: process not found.\n");
	return;
}

// checks if a process exists by pid
int exists(pid_t pid) {
	struct node* curr = queue_head;
	while (curr != NULL) {
		if (curr->pid == pid) {
			return 1;
		}
		curr = curr->next;
	}
	return 0;
}


/** User Input Parsing Functions **/

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
pid_t parse_pid (char* input) {
	if (input) {
		pid_t pid = atoi(input);
		if (pid) {
			return pid;
		}
	}
	
	printf("Error: invalid pid. Please enter an integer.\n");
	return -1;
}


/** Command Functions **/

void bg(char* program, char** more_args) {
	pid_t child_pid = fork();
	int status;
	
	// check if fork is successful
	if (child_pid >= 0) {
		// child process
		if (child_pid == 0) {   
			execvp(program, &more_args[0]);
			printf("Error: background process failed to start.\n");
			exit(1);
		// parent process
		} else {	
			printf("Started background process %s with pid %d\n",program, child_pid);
			insert(child_pid, program);
			sleep(3);
			
		}
	} else {
		printf("Error: fork failed.\n");
	}
	
}

void bglist() {
	int size = 0;
	
	if (queue_head) {
		struct node* curr = queue_head->next;
	
		printf("%s\n",queue_head->next->name);
		while (curr != NULL) {
			printf("%d:\t%s\n", curr->pid, curr->name);
			size++;
			curr = curr->next;
		}
	}
	
	printf("Total background jobs:\t%d\n", size);
}

void bgkill(pid_t pid) {
	int killed = kill(pid, SIGTERM);
	if (killed != -1) {
		delete(pid);
		sleep(3);
	} else {
		printf("Error: bgkill failed.\n");
	}
}

void bgstop(pid_t pid) {
	int stopped = kill(pid, SIGSTOP);
	if (stopped != -1) {
		sleep(3);
	} else {
		printf("Error: bgstop failed.\n");
	}
}

void bgstart(pid_t pid) {
	int started = kill(pid, SIGCONT);
	if (started != -1) {
		sleep(3);
	} else {
		printf("Error: bgstart failed.\n");
	}
}

void pstat(pid_t pid) {
	
	// HERE
}


/** Main Process Functions **/

// parses user input and runs command if possible
void run_input (char copy[]) {
	char* tok;
	tok = strtok (copy, " "); 
	
	// match command to a valid command
	int command = get_command(tok);
	
	// bg
	if (command == 0) {
		char* program = strtok(NULL," ");
		if (!program) {
			printf("Error: arg needed. Please enter the program you wish to run.\n");
		} else {
			char* more_args[MAX_INPUT];
			more_args[0] = program;

			//printf("%s\n",more_args[0]);
			
			int i = 1;
			while(program) {
				program = strtok(NULL," ");
				if (program) {
					more_args[i] = program;
					
					//printf("%s\n",more_args[i]);
					
					i++;
				}
			}
			i++;
			more_args[i] = NULL;
			
			bg(more_args[0], more_args);			
		}
		
	// bglist	
	} else if (command == 1) {
		char* not_empty = strtok(NULL," ");
		if (not_empty) {
			printf("Error: arg not needed for this command. Please try again.\n");
		} else {
			bglist();
		}
		
	// bgkill, bgstop, bgstart, or pstat 
	} else if (command > 1) {
		// parse for a valid pid
		pid_t target_pid = parse_pid(strtok(NULL," "));
			
		if (target_pid > -1){	
			//check if target process exists
			if (!exists(target_pid)) {
				printf("Error: Process %d does not exist.\n", target_pid);
				return;
			}
		
			if (command == 2) {
				// bgkill
				bgkill(target_pid);
			} else if (command == 3) {
				// bgstop
				bgstop(target_pid);
			} else if (command == 4) {
				// bgstart
				bgstart(target_pid);
			} else if (command == 5) {
				// pstat
				pstat(target_pid);
			} else {
				printf("Error: command invalid. How did you get here?\n");
				return;
				
			}		
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
			char copy[MAX_INPUT+1];
			strncpy (copy, input, MAX_INPUT);
			run_input(copy);
			
		}
		
		// check for status updates
		
		// HERE

		//printf("%s\n", input);
	}
	
    exit (0);
}