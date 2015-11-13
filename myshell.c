// Takumi House (houset@bu.edu) & Kristel Tan (ktan@bu.edu)
// CAS CS410 Advanced Software Systems
// Due: November 13, 2015

// myshell.c
// A basic shell program that parses, interprets, and executes commands

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_LINE 100
#define PARSE_TOKENS_BUF_SIZE 64

/* Intercepts SIGINT signal and sends it to foreground processes */

void sigint_handler(int pid) {
    int ret;
    ret = kill(pid, SIGHUP);
    printf("ret : %d ", ret);
}

/* Returns true if passed in character is a special character */

int isSpecialChar(char * c) {
    
    if (strcmp(c, ";") == 0 || strcmp(c, ">") == 0 || strcmp(c, "<") == 0 || strcmp(c, "1>") == 0 ||
       strcmp(c, "2>") == 0 || strcmp(c, "&>") == 0 || strcmp(c, "&") == 0) {
        return 1;
    } else {
        return 0;
    }
}

/* Returns true if passed in character is a special character */

int isFileOperator(char * c) {
    
    if (strcmp(c, ">") == 0 || strcmp(c, "<") == 0 || strcmp(c, "1>") == 0 || strcmp(c, "2>") == 0 || strcmp(c, "&>") == 0) {
        return 1;
    } else {
        return 0;
    }
}

/* Parses commands delimited by whitespaces and returns the total number of arguments */

int myshell_parse_line(char* *args, char* cmd) {
    
    int numArg = 0;
    int i;
    
    // Iterate through cmd array passed from main
    for (i = 0; i < MAX_LINE && cmd[i] != '\n'; i++) {
        
        int start;
        int end;
        
        // Increment 'end' until a newline character or space is reached
        while (cmd[end] != '\n' && cmd[end] != ' ') {
            end++;
        }
    
        int r = end - start + 1;                        // Range of substring of argument
        char * buffer = malloc(sizeof(char*) * r);      // Allocate memory for 'buffer' based on range
        
        // Return an error if memory allocation fails
        if (!buffer) {
            fprintf(stderr, "myshell: memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        
        // Copy contents of 'cmd + start' into 'buffer'
        memcpy(buffer, cmd + start, end - start);
        buffer[r] = '\0';
    
        args[numArg] = buffer;                          // Stores substring into 'args'
        i = end;                                        // Sets index to value of 'end'
    
        // Exit out of loop if newline character is reached because this indicates there are no more commands
        if (cmd[i] == '\n') {
            break;
        }
    }
    
    return numArg;
    
}

/* Launches shell and executes given arguments */

int myshell_launch(char **args, int numArg) {
    
    int waitForChildren = 1;
    
    // Check if command is followed by an ampersand
    if (strcmp(args[numArg] - 1, "&") == 0) {
        waitForChildren = 0;                        // Sets waitForChildren to 'false'
        args[numArg - 1] = NULL;
        numArg = numArg - 1;                        // Decrement 'numArg'
    }
    
    int numSpecialChars = 0;                        // Initialize special character count to 0
    int cmds = 1;                                   // If this point is reached, at least minimum number of commands is 1
    int cmdStarts[numArg];                          // Stores indices of every new command
    cmdStarts[0] = 0;                               // First command's index should always be 0
    
    char * specialChars[numArg];                    // Will store special characters
    
    // Iterates through 'args' to look for special characters
    int i, j;
    for(i = 0, j = 1; i < numArg; i++, j++) {
        if (isSpecialChar(args[i])) {
            cmdStarts[j] = i + 1;                   // Mark another command start index to 'cmdStarts' array
            numSpecialChars++;                      // Increment special character count
            cmds++;                                 // Increment command count
            specialChars[j - 1] = args[i];          // Store the special character into 'specialChars'
            args[i] = NULL;                         // Update current index of args to NULL because special character was accounted for
        }
    }
    
    int fds[2 * numSpecialChars];                   // There should be 2 file descriptors for every special character
    
    for (i = 0; i < numSpecialChars; i++) {
        
        // Checks for file redirection operators
        if (isFileOperator(specialChars[i])) {
            
            // Opens a new file and obtains its file descriptor
            if ((strcmp(specialChars[i], ">") == 0) || (strcmp(specialChars[i], "1>")) || (strcmp(specialChars[i], "2>"))) {
                fds[i * 2] = open(args[cmdStarts[i + 1]], O_WRONLY | O_CREAT, S_IRWXO | S_IRWXG | S_IRWXU);
            }
            else if ((strcmp(specialChars[i], "&>")) == 0) {
                fds[i * 2] = open(args[cmdStarts[i + 1]], O_WRONLY | O_APPEND | O_CREAT, S_IRWXO | S_IRWXG | S_IRWXU);
            }
            else if ((strcmp(specialChars[i], "<")) == 0) {
                fds[i * 2] = open(args[cmdStarts[i + 1]], O_RDONLY);
            }
            
            cmdStarts[i + 1] = -1;
            fds[i * 2 + 1] = -1;
            
            // Return an error if opening file fails
            if (fds[i * 2] < 0) {
                fprintf(stderr, "myshell: file could not be opened\n");
                exit(EXIT_FAILURE);
            }
            // Return an error if opening pipe fails
            if (pipe(fds + i * 2) < 0) {
                fprintf(stderr, "myshell: pipe could not be opened\n");
                exit(EXIT_FAILURE);
            }
                                                                                                            
        }
    }
    
    pid_t pid;                              // Initialize pid variable of pid_t data type (will represent a process ID)
    int currentCmd;
    
    for (i = 0, j = 0; i < cmds; ++i) {
        
        currentCmd = cmdStarts[i];          // Grab a command start index and assign it to 'currentCmd'
        
        // Jump back to top of for loop if current command index is not valid
        if (currentCmd < 0) {
            continue;
        }
        
        // Assign return value of fork() to pid
        pid = fork();
        
        // If call to fork() returns a 0 to the newly created child process, handle file descriptors accordingly
        if (pid == 0) {
            
            signal(SIGINT, sigint_handler);
            
            if (i > 0 && (strcmp(specialChars[i - 1], "|") == 0)) {
                if (dup2(fds[j - 2], 0) < 0) {
                    fprintf(stderr, "myshell: file descriptor could not be duplicated\n");
                    exit(EXIT_FAILURE);
                }
            }
            
            if (i < numSpecialChars) {
                if (((strcmp(specialChars[i], ">") == 0) || (strcmp(specialChars[i], "1>") == 0)) && (dup2(fds[j], 1) < 0)) {
                    fprintf(stderr, "myshell: file descriptor could not be duplicated\n");
                    exit(EXIT_FAILURE);
                }
                else if ((strcmp(specialChars[i], "2>")) == 0) {
                    dup2(fds[j], 2);
                }
                else if ((strcmp(specialChars[i], "&>")) == 0) {
                    // Redirect both stdout and stderr
                    dup2(fds[j], 1);
                    dup2(fds[j], 2);
                }
                else if ((strcmp(specialChars[i], "<")) == 0) {
                    dup2(fds[j], 0);
                    if ((i + 1 < numSpecialChars) && (strcmp(specialChars[i + 1], ">")) == 0) {
                        dup2(fds[j + 2], 1);
                    }
                }
                else if ((strcmp(specialChars[i], "|")) == 0) {
                    if(dup2(fds[j + 1], 1) < 0){
                        fprintf(stderr, "myshell: file descriptor could not be duplicated\n");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            
            // Closes all file descriptors in the child process once all file descriptors are duplicated
            int f;
            for (f = 0; f < 2 * numSpecialChars; f++) {
                if (fds[f] > 0) {
                    close(fds[f]);
                }
            }
            
            // Overwrites the caller's address space with the current command and starts the execution
            // Returns an error if the execution fails
            if (execvp(args[currentCmd], args + currentCmd) < 0) {
                fprintf(stderr, "myshell: exec failed\n");
                exit(EXIT_FAILURE);
            }
        }
        
        // Returns an error if the creation of the child process was unsuccessful
        else if (pid < 0) {
            fprintf(stderr, "myshell: creation of child process was unsuccessful\n");
            exit(EXIT_FAILURE);
        }
        
        j += 2;
    }
    
    // Closes all open file descriptors
    int k;
    for (k = 0; k < 2 * numSpecialChars; k++) {
        if (fds[k] > 0) {
            close(fds[k]);
        }
    }
    
    // Wait for all children to finish executing
    while (1) {
        int status;
        pid_t done = wait(&status);
        if (done == -1) {
            break;      // No more child processes
        }
    }
    return 0;
}

int main(void) {
    
    char* args[MAX_LINE / 2 + 1];                           // Stores command line arguments
    
    // Loops forever until terminated or interrupted
    while (1) {
        
        signal(SIGINT, sigint_handler);                     // Checks for CTRL+C signal interrupt generated by user
        
        // Iterates through 'args', clearing any leftover commands
        int i;
        for (i = 0; i < MAX_LINE / 2 + 1; i++) {
            args[i] = NULL;
        }
        
        // Terminate myshell program if CTRL+D is typed
        if (feof(stdin) != 0) {
            return 0;
        }
        
        printf("myshell> ");								// Prompt for user to type in the shell command
        fflush(stdout);										// Clear the output buffer
        
        char cmd[MAX_LINE];
        fgets(cmd, MAX_LINE, stdin);						// Stores the first 100 characters of the command into 'cmd'
        int numArgs = myshell_parse_line(args, cmd);		// Parses inputted line and returns number of arguments
        
        // Executes a sequence of two commands one after the other if semicolon is used
        // This functionality was not implemented properly
        /*
        if ((strcmp(args[1], ";")) == 0) {
            myshell_launch(&args[0], 1);
            myshell_launch(&args[2], 1);
            return 0;
        }*/
        
        myshell_launch(args, numArgs);						// Launches shell and executes given arguments
    }
    return 0;
}


