// Shell starter file
// You may make any changes to any part of this file.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include  <sys/types.h>
#include <sys/wait.h>

#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define HISTORY_DEPTH 100

// used to store commands
char history[HISTORY_DEPTH][COMMAND_LENGTH];
int count=0;


/* ---------- HISTORY FUNCTIONS ----------- */


void remember(char *buff, _Bool in_background) {
  strcpy(history[count], buff);
  count++;
}



// insert into history array
/*
void remember(char *tokens[], _Bool in_background) {
  int i = 0;
  char input[COMMAND_LENGTH] = "";

  while (tokens[i] != 0) {
    if (i == 0) {
      strcat(input, tokens[i]);
      i++;
      continue;
    } else {
      strcat(input, " ");
      strcat(input, tokens[i]);
      i++;
    }
  }

  if (in_background) {
    strcat(input, " &");
  }

  strcpy(history[count], input);
  count++;
}
*/


// print last 10 or fewer commands
void recall() {
  if (count <= 10) {
    for (int i=0; i < count; i++) {
      char index[10];
      sprintf(index, "%d", i + 1);
      write(STDOUT_FILENO, index, strlen(index));
      write(STDOUT_FILENO, "\t", strlen("\t"));
      write(STDOUT_FILENO, history[i], strlen(history[i]));
      write(STDOUT_FILENO, "\n", strlen("\n"));
    }
  } else {
    int i = count - 10;
    while (i < count) {
      char index[10];
      sprintf(index, "%d", i + 1);
      write(STDOUT_FILENO, index, strlen(index));
      write(STDOUT_FILENO, "\t", strlen("\t"));
      write(STDOUT_FILENO, history[i], strlen(history[i]));
      write(STDOUT_FILENO, "\n", strlen("\n"));
      i++;
    }
  }
}
/* ----------- HISTORY ENDS HERE -------------*/


/**
 * Command Input and Processing
 */

/*
 * Tokenize the string in 'buff' into 'tokens'.
 * buff: Character array containing string to tokenize.
 *       Will be modified: all whitespace replaced with '\0'
 * tokens: array of pointers of size at least COMMAND_LENGTH/2 + 1.
 *       Will be modified so tokens[i] points to the i'th token
 *       in the string buff. All returned tokens will be non-empty.
 *       NOTE: pointers in tokens[] will all point into buff!
 *       Ends with a null pointer.
 * returns: number of tokens.
 */
int tokenize_command(char *buff, char *tokens[])
{
	int token_count = 0;
	_Bool in_token = false;
	int num_chars = strnlen(buff, COMMAND_LENGTH);
	for (int i = 0; i < num_chars; i++) {
		switch (buff[i]) {
		// Handle token delimiters (ends):
		case ' ':
		case '\t':
		case '\n':
			buff[i] = '\0';
			in_token = false;
			break;

		// Handle other characters (may be start)
		default:
			if (!in_token) {
				tokens[token_count] = &buff[i];
				token_count++;
				in_token = true;
			}
		}
	}
	tokens[token_count] = NULL;
	return token_count;
}

/**
 * Read a command from the keyboard into the buffer 'buff' and tokenize it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 *       COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 *       least NUM_TOKENS long. Will strip out up to one final '&' token.
 *       tokens will be NULL terminated (a NULL pointer indicates end of tokens).
 * in_background: pointer to a boolean variable. Set to true if user entered
 *       an & as their last token; otherwise set to false.
 */
void read_command(char *buff, char *buff_history, char *tokens[], _Bool *in_background)
{
	*in_background = false;

	// Read input
	int length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);


	if (length < 0) {
		perror("Unable to read command from keyboard. Terminating.\n");
		exit(-1);
	}

	// Null terminate and strip \n.
	buff[length] = '\0';
	if (buff[strlen(buff) - 1] == '\n') {
		buff[strlen(buff) - 1] = '\0';
	}
  strcpy(buff_history, buff);

	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		return;
	}

	// Extract if running in background:
	if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
		*in_background = true;
		tokens[token_count - 1] = 0;
	}
}

/**
 * Main and Execute Commands
 */
int main(int argc, char* argv[])
{

  char cwd[PATH_MAX];
	char input_buffer[COMMAND_LENGTH];
  char input_buffer_history[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];
	while (true) {

    // get the current working directory right away and handle error
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
      write(STDOUT_FILENO, "getcwd() error\n", strlen("getcwd() error\n"));
    }
    // concatenate cwd with prompt
    strcat(cwd, "> ");

		// Get command
		// Use write because we need to use read() to work with
		// signals, and read() is incompatible with printf().
		write(STDOUT_FILENO, cwd, strlen(cwd));
		_Bool in_background = false;
		read_command(input_buffer, input_buffer_history , tokens, &in_background);

    // Cleanup any previously exited background child processes
    // (The zombies)
    while (waitpid(-1, NULL, WNOHANG) > 0)
      ; // do nothing.

    /* ---------INTERNAL COMMANDS START HERE---------- */

    // handle empty input
    if (tokens[0] == 0) {
      continue;
    }

    // exit program
    if (strcmp(tokens[0], "exit") == 0) {
      exit(0);
    }

    // print the current working directory
    if (strcmp(tokens[0], "pwd") == 0) {
      char cwd[1024];
      getcwd(cwd, sizeof(cwd));
      write(STDOUT_FILENO, cwd, strlen(cwd));
      write(STDOUT_FILENO, "\n", strlen("\n"));
      remember(input_buffer_history, in_background);
      continue;
    }

    // change the current working directory
    if (strcmp(tokens[0], "cd") == 0) {
      if (tokens[1] == 0) {
        write(STDOUT_FILENO, "No such file or directory\n", strlen("No such file or directory\n"));
      } else {
        if (chdir(tokens[1]) != 0) {
          write(STDOUT_FILENO, "No such file or directory\n", strlen("No such file or directory\n"));
        }
      }
      remember(input_buffer_history, in_background);
      continue;
    }

    if (strcmp(tokens[0], "history") == 0) {
      remember(input_buffer_history, in_background);
      recall();
      continue;
    }

    /* ------------INTERNAL COMMANDS END HERE------------- */

		// DEBUG: Dump out arguments:
		for (int i = 0; tokens[i] != NULL; i++) {
			write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
			write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
			write(STDOUT_FILENO, "\n", strlen("\n"));
		}
		if (in_background) {
			write(STDOUT_FILENO, "Run in background.\n", strlen("Run in background.\n"));
		}

		/**
		 * Steps For Basic Shell:
		 * 1. Fork a child process
		 * 2. Child process invokes execvp() using results in token array.
		 * 3. If in_background is false, parent waits for
		 *    child to finish. Otherwise, parent loops back to
		 *    read_command() again immediately.
		 */
     else {
       // try and create a child process
       int status;
       pid_t pid;
       pid = fork();
       remember(input_buffer_history, in_background);

       if (pid < 0) {
         write(STDOUT_FILENO, "Fork failed.\n", strlen("Fork failed.\n"));
         exit(1);
       } else if (pid == 0) { // child process
          if (execvp(tokens[0], tokens) == -1) {
             write(STDOUT_FILENO, tokens[0], strlen(tokens[0]));
             write(STDOUT_FILENO, ": Unknown command.\n", strlen(": Unknown command.\n"));
             exit(1);
          }
      } else {
          if (!in_background) {
            waitpid(pid, &status, WUNTRACED);
          }
      }
     }
	}
	return 0;
}
