// Shell starter file
// You may make any changes to any part of this file.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define HISTORY_DEPTH 10

// used to store commands
char history[HISTORY_DEPTH][COMMAND_LENGTH];
int count=0;



/* ---------- HISTORY FUNCTIONS ----------- */

void remember(char *buff) {
  strcpy(history[count%HISTORY_DEPTH], buff);
  count++;
}


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
    int i = count - HISTORY_DEPTH;
    while (i < count) {
      char index[10];
      sprintf(index, "%d", i + 1);
      write(STDOUT_FILENO, index, strlen(index));
      write(STDOUT_FILENO, "\t", strlen("\t"));
      write(STDOUT_FILENO, history[i%HISTORY_DEPTH], strlen(history[i%HISTORY_DEPTH]));
      write(STDOUT_FILENO, "\n", strlen("\n"));
      i++;
    }
  }
}
/* ----------- HISTORY ENDS HERE -------------*/

/* Signal handler function */
void handle_SIGINT() {
      write(STDOUT_FILENO, "\n", strlen("\n"));
      recall();
}


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
void read_command(char *buff, char *buff_history, char *tokens[], _Bool *in_background, _Bool *prev_cmd)
{
	*in_background = false;
  int length;


	// Read input
  if (*prev_cmd == false) {
	   length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);
  }

  if (*prev_cmd == true) {
    length = strlen(buff);
  }


  if ( (length < 0) && (errno !=EINTR) ){
        perror("Unable to read command. Terminating.\n");
        exit(-1);  /* terminate with error */
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

  /* set up the signlan handler */
  struct sigaction handler;
  handler.sa_handler = handle_SIGINT;
  handler.sa_flags = 0;
  sigemptyset(&handler.sa_mask);
  sigaction(SIGINT, &handler, NULL);

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
    _Bool previous = false;
    input_buffer[0] = '\0';
    input_buffer_history[0] = '\0';
		read_command(input_buffer, input_buffer_history , tokens, &in_background, &previous);



    /* ---------INTERNAL COMMANDS START HERE---------- */

    // handle empty input
    if (tokens[0] == 0) {
      continue;
    }

    // handling !
    if (*tokens[0] == '!' && (strlen(tokens[0]) != 1)) {

      if (atoi(&tokens[0][1]) == 0 && strcmp(tokens[0], "!!") != 0) {
        write(STDOUT_FILENO, "SHELL: Unknown history command.\n", strlen("SHELL: Unknown history command.\n"));
        continue;
      }

      if (strcmp(tokens[0], "!!") == 0) {
        if (count > 0) {
          previous = true;
          strcpy(input_buffer, history[(count-1)%HISTORY_DEPTH]);
          write(STDOUT_FILENO, input_buffer, strlen(input_buffer));
          write(STDOUT_FILENO, "\n", strlen("\n"));
          read_command(input_buffer, input_buffer_history, tokens, &in_background, &previous);
        } else {
            write(STDOUT_FILENO, "SHELL: Unknown history command.\n", strlen("SHELL: Unknown history command.\n"));
            continue;
        }
      }

      if (atoi(&tokens[0][1]) != 0) {
        int hist = atoi(&tokens[0][1]);
        if (hist <= 0) {
          write(STDOUT_FILENO, "SHELL: Unknown history command.\n", strlen("SHELL: Unknown history command.\n"));
          continue;
        }
        if (hist <= count && hist >= (count - 9)) {
          previous = true;
          strcpy(input_buffer, history[(hist-1)%HISTORY_DEPTH]);
          write(STDOUT_FILENO, input_buffer, strlen(input_buffer));
          write(STDOUT_FILENO, "\n", strlen("\n"));
          read_command(input_buffer, input_buffer_history, tokens, &in_background, &previous);
        } else {
          write(STDOUT_FILENO, "SHELL: Unknown history command.\n", strlen("SHELL: Unknown history command.\n"));
          continue;
        }
      }
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
      remember(input_buffer_history);
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
      remember(input_buffer_history);
      continue;
    }

    // print out the history
    if (strcmp(tokens[0], "history") == 0) {
      remember(input_buffer_history);
      recall();
      continue;
    }

    /* ------------INTERNAL COMMANDS END HERE------------- */

		// DEBUG: Dump out arguments:
    /*

		for (int i = 0; tokens[i] != NULL; i++) {
			write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
			write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
			write(STDOUT_FILENO, "\n", strlen("\n"));
		}
		if (in_background) {
			write(STDOUT_FILENO, "Run in background.\n", strlen("Run in background.\n"));
		}
    */


		/**
		 * Steps For Basic Shell:
		 * 1. Fork a child process
		 * 2. Child process invokes execvp() using results in token array.
		 * 3. If in_background is false, parent waits for
		 *    child to finish. Otherwise, parent loops back to
		 *    read_command() again immediately.
		 */

     // try and create a child process
     write(STDOUT_FILENO, "hello\n", strlen("hello\n"));
     int status;
     pid_t pid;
     pid = fork();
     remember(input_buffer_history);

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
          write(STDOUT_FILENO, "hello2\n", strlen("hello2\n"));
          waitpid(pid, &status, WUNTRACED);
          continue;
        }
    }
	}
	return 0;
}
