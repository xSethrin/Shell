/**
	Nikolo Sperberg
	Sterling Rohlinger
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


// buffer is the input string of characters
// args is the output array of arguments. It has already been created with argsSize slots.
// nargs as the number filled in that is passed back
void parseArgs(char *buffer, char** args, int argsSize, int *nargs) {
	char *bufArgs[argsSize];
	char **cp;
	char *wbuf;
	int i, j;
	wbuf = buffer;
	bufArgs[0] = buffer;
	args[0] = buffer;
	for(cp = bufArgs; (*cp = strsep(&wbuf, " \n\t")) != NULL;){
		if((*cp != '\0') && (++cp >= &bufArgs[argsSize])){
			break;
		}
	}
	for (j = i = 0; bufArgs[i] != NULL; i++){
	
	if(strlen(bufArgs[i]) > 0){
			args[j++] = bufArgs[i];
		}
	}
	// Add the NULL as the end argument because we need that for later
	*nargs = j;
	args[j] = NULL;
}

//method to display CWD
void displayCWD(){
	char cwd[300];
    getcwd(cwd, sizeof(cwd));
	printf("%s>", cwd);
}

//method that deletes tokens
void deleteToken(char** args, int i, int* nargs){
	while(args[i + 1] != NULL){
		args[i] = args[i + 1];
		i++;
	}
	*nargs = *nargs - 1;
}

//method that searches for input command  can we delete here???
void inputSearch(char** args, int* nargs){
	int i = 0;
	while(i < *nargs){
		if(strcmp(args[i], "<") == 0){
			freopen(args[i + 1], "w", stdout);
			deleteToken(args, i + 1, nargs);
			deleteToken(args, i, nargs);
		}
		i++;
	}
}

//method to search for output command
void outputSearch(char** args, int* nargs){
	int i = 0;
	while(i < *nargs){
		if(strcmp(args[i], ">") == 0){
			freopen(args[i + 1], "r", stdin);
			deleteToken(args, i + 1, nargs);
			deleteToken(args, i, nargs);			
		}
		i++;
	}
}

//method to find &
void backgroundSearch(char** args, int* nargs){ 
	int i = *nargs - 1;
	if(strcmp(args[i], "&") == 0){
		deleteToken(args, i, nargs);
	}
}

//method to change directory
void changeD(char** args){
	if(chdir(args[1]) != 0){
		printf("directory does not exist\n");
	}
}

//singal method handler
void handler(int i){
	int stat;
	int pid = waitpid(-1, &stat, WNOHANG);
	if(pid != -1){
		printf("waiting for child %d\n", pid);
		
	}
}

//method to execute command 
void execute(char** args, int nargs){
	signal(SIGCHLD, handler);
	int stat;
	int rc; 
	int pid = fork();
	if(pid == -1){
		printf("unable to fork\n");
		exit(1);
	}
	else if (pid == 0){
		inputSearch(args, &nargs);
		outputSearch(args, &nargs);
		backgroundSearch(args, &nargs);
		rc = execvp(args[0], args);
		if(rc != 0){
			if(strcmp(args[0], "exit") != 0){
				printf("unknown command\n");
			}
			exit(1);
		}
	}
	else{
		if(strcmp(args[nargs - 1], "&") != 0){
			waitpid(pid, &stat, 0);
		}
		else{
			printf("moveing %d to background\n", pid);
		}
	}
}

//main method of unix shell
int main(){
	bool run = true;
	char* buffer = malloc(sizeof(char) * 100);
	char** args = malloc(sizeof(char*) * 100);
	int nargs;
	while(run){
		displayCWD();
		fgets(buffer, 100, stdin);
		parseArgs(buffer, args, 100, &nargs);
		if(args[0] == NULL){
			//do nothing, this way there is no seg fault if no text is entered
		}
		else if(strcmp(args[0], "exit") == 0){
			printf("exiting\n");
			run = false;
			}
		else if(strcmp(args[0], "cd") == 0){
			changeD(args);
		}
		else {
			if(nargs > 0){
				execute(args, nargs);
			}
		}		
	}
	return 0;
}










