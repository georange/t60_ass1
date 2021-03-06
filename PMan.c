#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>         
#include <sys/wait.h>          
#include <signal.h> 

#include <readline/readline.h>
#include <readline/history.h>

#define MAX_INPUT 513
#define MAX_COMMAND 6
#define MAX_FILE 1025

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
		
		queue_head->pid = pid;
		queue_head->name = malloc(strlen(name)+1);
		strcpy(queue_head->name,name);
		queue_head->next = NULL;
		
		//printf("%s\n",queue_head->next->name);
		
	} else {
		struct node *curr = queue_head;
		while (curr->next != NULL) {
			curr = curr->next;
		}

		curr->next = (struct node*)malloc(sizeof(struct node));
		curr->next->pid = pid;
		curr->next->name = malloc(strlen(name)+1);
		strcpy(curr->next->name,name);
		curr->next->next = NULL;
		
		//printf("%s\n",curr->next->name);
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
			free(curr->name);
			free(curr);
			return;
		}
		prev = curr;
		curr = curr->next;
	}
	
	//printf("Error: process not found.\n");
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


/** Main Command Functions **/

void bg(char* program, char* more_args[]) {
    int status;
	int opts = WNOHANG | WUNTRACED | WCONTINUED;
	
	pid_t child_pid = fork();
	
	// check if fork is successful
	if (child_pid >= 0) {
		// child process
		if (child_pid == 0) {   
			execvp(program, more_args);
			printf("Error: background process failed to start.\n");
			exit(1);
		// parent process
		} else {	
			waitpid(child_pid, &status, opts);
			if (WEXITSTATUS(status) != 1) {
				printf("Started background process %s with pid %d\n",program, child_pid);
				insert(child_pid, program);
				sleep(3);
			}		
		}
	} else {
		printf("Error: fork failed.\n");
	}
	
}

void bglist() {
	int size = 0;
	
	if (queue_head) {
		struct node* curr = queue_head;
	
		//printf("%s\n",queue_head->next->name);
		while (curr != NULL) {
			printf("%d:\t%s\n", curr->pid, curr->name);
			size++;
			curr = curr->next;
		}
	}
	
	printf("Total background jobs:\t%d\n", size);
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

void bgkill(pid_t pid) {
	// restart process to kill
	bgstart(pid);
	
	int killed = kill(pid, SIGTERM);
	if (killed != -1) {
		delete(pid);
		sleep(3);
	} else {
		printf("Error: bgkill failed.\n");
	}
}

void pstat(pid_t pid) {
	if (exists(pid)) {
		char stat_path[MAX_INPUT];
		char status_path[MAX_INPUT];	
		sprintf(stat_path, "/proc/%d/stat", pid);
		sprintf(status_path, "/proc/%d/status", pid);
	
		// read and save contents of stat file in a list
		FILE* stat_file = fopen(stat_path, "r");	
		if (!stat_file)	{
			printf("Error: could not open stat file.");
			return;
		}
		
		char stat_buffer[MAX_FILE];
		char* stats[MAX_INPUT];
		char* stat_tok;
		int i = 0;
		while (fgets(stat_buffer, MAX_FILE-1, stat_file)) {
			stat_tok = strtok(stat_buffer, " ");
			//stats[i] = stat_tok;
			while (stat_tok) {
				stats[i] = stat_tok;
				stat_tok = strtok(NULL, " ");
				i++;
			}
		}
		fclose(stat_file);
		
		FILE* status_file = fopen(status_path, "r");	
		if (!status_file)	{
			printf("Error: could not open status file.");
			return;
		}
		
		// read and save necessary statuses
		char status_buffer[MAX_FILE];
		char voluntary_ctxt_switches[MAX_FILE];
		char nonvoluntary_ctxt_switches[MAX_FILE];
		while (fgets(status_buffer, MAX_FILE-1, status_file)) {
			if (!strncmp(status_buffer, "voluntary_ctxt_switches", strlen("voluntary_ctxt_switches"))) {
				//voluntary_ctxt_switches = status_buffer;
				strncpy(voluntary_ctxt_switches, status_buffer, strlen(status_buffer)+1);
			} else if (!strncmp(status_buffer, "nonvoluntary_ctxt_switches", strlen("nonvoluntary_ctxt_switches"))) {
				//nonvoluntary_ctxt_switches = status_buffer;
				strncpy(nonvoluntary_ctxt_switches, status_buffer, strlen(status_buffer)+1);
			}
		}
		fclose(status_file);
		
		// print out results
		double utime = atof(stats[13])/sysconf(_SC_CLK_TCK);
	    double stime = atof(stats[14])/sysconf(_SC_CLK_TCK);
		
		printf("comm:\t%s\n", stats[1]);
		printf("state:\t%s\n", stats[2]);
		printf("utime:\t%lf\n", utime);
		printf("stime:\t%lf\n", stime);
		printf("rss:\t%s\n", stats[24]);
		
		printf("%s", voluntary_ctxt_switches);
		printf("%s", nonvoluntary_ctxt_switches);
		
	} else {
		printf("Error: process %d does not exist.\n", pid);
	}
}


/** Main Process Functions **/

// parses user input and runs command if possible
void run_input (char* copy) {
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

void check_status() {
	pid_t pid;
	int status;
	int opts = WNOHANG | WUNTRACED | WCONTINUED;
	
	while(1) {
		pid = waitpid(-1, &status, opts);
		if (pid > 0) { 

			if (WIFEXITED(status)) {
				delete(pid);
				printf("Background process %d has been terminated.\n", pid); 
			} else if (WIFSIGNALED(status)) {
				printf("Background process %d has been killed.\n", pid);
			} else if (WIFSTOPPED(status)) {
				printf("Background process %d has been paused.\n", pid);
			} else if (WIFCONTINUED(status)) {    
				printf("Background process %d has been continued.\n", pid);   
			}
			sleep(1);
			
		} else {
			break;
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
		check_status();

		//printf("%s\n", input);
	}
	
    exit (0);
}