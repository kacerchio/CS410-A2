// Takumi House (houset@bu.edu) & Kristel Tan (ktan@bu.edu)
// CAS CS410 Advanced Software Systems
// Due: November 11, 2015

// myshell.c 
// A basic shell program that parses, interprets, and executes commands

#include <stdlib.h>

#define READ_LINE_BUF_SIZE 1024
#define PARSE_TOKENS_BUF_SIZE 64
#define PARSE_SPACES " \t\n"

int main(int argc, char ** argv) {

	shell_loop();
	return EXIT_SUCCESS;
}

void shell_loop(void) {

	char *line;
	char **args;
	int status;

	do {

		printf("myshell> ");			// Prompt for user to type in the shell command
		line = shell_read_line();		// Read a line from standard input
		args = shell_parse_line();		// Parse the command
		status = shell_exec(args);		// 

		free(line);
		free(args);

	} while(status);
}

char *read_line(void) {

	int bufsize = READ_LINE_BUF_SIZE;					// Initialize buffer size to constant READ_LINE_BUF_SIZE
	char *buffer = malloc(sizeof(char) * bufsize);		// Pointer to allocated memory for buffer
	int index = 0;
	int c;

	if (!buffer) {
		fprintf(stderr, "myshell: buffer allocation failed\n");
		return EXIT_FAILURE;
	}

	while(1) {

		// Reads a character from standard input
		c = getchar();

		// If character is end of file or new line, null terminate the current string
		// Else, add the character to buffer
		if (c == EOF || c == '\n') {
			buffer[index] = '\0';
			return buffer;
		} else {
			// If character is not end of file or new line, add it to the buffer
			buffer[index] = c;
		}

		index++;		// Increment to next index

		// If index exceeds the buffer size, reallocate memory
		if(index >= bufsize) {
			bufsize += READ_LINE_BUF_SIZE;
			buffer = realloc(buffer, bufsize); 
			if (!buffer) {
				fprintf(stderr, "myshell: buffer reallocation failed\n");
				return EXIT_FAILURE;
			}
		}
	}
}

char **shell_parse_line(*char line) {

	int bufsize = PARSE_TOKENS_BUF_SIZE;
	char **tokens = malloc(sizeof(char *) * bufsize);
	int index = 0;
	char *token;

	if (!tokens) {
		fprintf(stderr, "myshell: memory allocation failed\n");
		return EXIT_FAILURE;
	}

	// Returns pointer to the first token
	token = strtok(line, PARSE_SPACES);

	// While loop continues as long as strtok (at the end of the previous loop) returns a non-NULL value
	while(token != NULL) {
		tokens[index] = token;
		index++;

		if(index >= bufsize) {
			bufsize += PARSE_TOKENS_BUF_SIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "myshell: memory reallocation failed\n");
				return EXIT_FAILURE;
			}
		}
		token = strtok(NULL, PARSE_SPACES);
	}
	tokens[index] = NULL;
	return tokens;
}







