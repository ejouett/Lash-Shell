#include <stdlib.h>  //everything in the checklist works
#include <stdio.h>	//have a parsed path slashed out since everything in check list works w/o it??
#include <string.h>
#include <unistd.h>    //I use string token to parse the command and path
#include <sys/wait.h>

int cd(char *);

int main(int argc, char **argv) {
    char **command, **tmp;
    char *input;
    pid_t child_pid;
    int stat_loc;
    size_t bufsize = 100;
    size_t characters;
	char *path = "/bin/";
	//char *path = "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin";

	 while (1) {
		printf("$ ");
		input = malloc(bufsize * sizeof(char ));
		characters = getline(&input, &bufsize, stdin);
		input[strlen(input)-1] = '\0';
		char *seperator = " ";
		char *token;
		int index = 0;
		token = strtok(input, seperator);
		int nums = 1;
		
		for (unsigned int k = 0; k < strlen(input); k++){
			if (input[k] == ' '){
			nums++;
			}
		}
		command = malloc(bufsize * nums * sizeof(char ));  //parse input string using strtoken
		while (token != NULL){						//populate command
	    		command[index] = malloc(bufsize * sizeof(char *));
	   		strcpy(command[index], token);
	    		index++;

	    		token = strtok(NULL, seperator);
		}

		
		char *pathcmd = malloc((strlen(path) + strlen(command[0])) * sizeof(char ));
		
		strcat(pathcmd, path);
		strcat(pathcmd, command[0]);
  		tmp = malloc(bufsize * nums * sizeof(char ));

  		for (int i = 1; command[i] != NULL; i++){
			tmp[i] = malloc(bufsize * sizeof(char ));
			strcpy(tmp[i], command[i]);
		}

		tmp[0] = malloc(bufsize * sizeof(char ));
		strcpy(tmp[0], pathcmd);
  		
          	if (!command[0]) {      /* Handle empty commands */
              	free(input);
              	free(command);
              	continue;
          	}

  
          	if (strcmp(command[0], "cd") == 0) {  //cd command 
              		if (chdir(command[1]) < 0) {
                 	 perror(command[1]);
              }
  
              /* Skip the fork */
              	continue;
          	}
		
		if (strcmp(command[0], "exit") == 0){  //exit command
			exit(0);
	} 
  	
		if(strcmp(command[0], "path") == 0){			//path commands
			if(strcmp(command[1], "print") == 0){
				printf("%s\n", path);		
			}else if(strcmp(command[1], "add") == 0){
				char * new_path = malloc((strlen(path) + strlen(command[2])+1) * sizeof(char ));
				strcpy(new_path, path);
				strcat(new_path, ":");
				strcat(new_path, command[2]);
				path = new_path; //need to free old path - memory leak
			}else if(strcmp(command[1], "remove") == 0){
				char *delim = ":";
				char *token;
				int found = 0;
				token = strtok(path, delim);
			 	char *newpath = malloc((strlen(path)+1) * sizeof(char ));
	        	        while (token != NULL){
        	                	if (strcmp(token, command[2]) == 0){
						found = 1;
					}else {
						strcat(newpath, ":");
						strcat(newpath, token);
					}
		                       token = strtok(NULL, delim);
                		}
				if (found == 0){
					perror("error path not found");
				}
				path = newpath;
			}else if(strcmp(command[1], "clear") == 0){ //memory issues need to free path
			//	free(path);
				path = "";
			}
		 continue;
		}
          	child_pid = fork();
          	if (child_pid < 0) {
              		perror("Fork failed");
              		exit(1);
          }
  
 	 	if (child_pid == 0) {
              /* Never returns if the call is successful */
        	 	 if (execvp(tmp[0], tmp) < 0) {
                	  	perror(tmp[0]);
                 		 exit(1);
          	    }
       		 } else {
             	 waitpid(child_pid, &stat_loc, WUNTRACED);
        	}
  
       		 free(input);
	  	free(tmp);
   	 }

    return 0;
	}
	
	int cd(char *path) {
    		return chdir(path);
}
