#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)

#define HISTORY_LENGTH 1024
#define HISTORY_DEPTH 10

char history[HISTORY_DEPTH][HISTORY_LENGTH];

int command_count = 0;

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
    for (int i = 0; i < num_chars; i++)
    {
        switch (buff[i])
        {
        // Handle token delimiters (ends):
        case ' ':
        case '\t':
        case '\n':
            buff[i] = '\0';
            in_token = false;
            break;

        // Handle other characters (may be start)
        default:
            if (!in_token)
            {
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
void read_command(char *buff, char *tokens[], _Bool *in_background)
{
    *in_background = false;

    // Read input
    int length = read(STDIN_FILENO, buff, COMMAND_LENGTH - 1);

    if (length < 0)
    {
        perror("Unable to read command from keyboard. Terminating.\n");
        exit(-1);
    }

    // Null terminate and strip \n.
    buff[length] = '\0';
    if (buff[strlen(buff) - 1] == '\n')
    {
        buff[strlen(buff) - 1] = '\0';
    }

    // Tokenize (saving original command string)
    int token_count = tokenize_command(buff, tokens);
    if (token_count == 0)
    {
        return;
    }

    // Extract if running in background:
    if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0)
    {
        *in_background = true;
        tokens[token_count - 1] = 0;
    }
}
void add_to_history(char *tokens[], _Bool in_background)
{

    if (tokens[0][0] != '!')
    {
        command_count += 1;

        int k = 0;
        for (int i = 0; (tokens[i] != NULL); i++)
        {

            for (int j = 0; (j < strlen(tokens[i])); j++)
            {
                history[command_count][k++] = tokens[i][j];
            }
            history[command_count][k++] = ' ';
        }
        //If the command is run in the background using &, it must be added to the history with the &.
        if (in_background)
        {
            history[command_count][k] = '&';
        }
    }
}
/**
 * need to clean the code here
 */
void show_history()
{

    if (command_count >= 10)
    {
        for (int i = command_count - 9; i <= command_count; i++)
        {
            char index[10];
            sprintf(index, "%d", i);
            write(STDOUT_FILENO, index, strlen(index));
            write(STDOUT_FILENO, "\t", strlen("\t"));
            write(STDOUT_FILENO, history[i], strlen(history[i]));
            write(STDOUT_FILENO, "\n", strlen("\n"));
        }
    }
    else
    {
        for (int i = 1; i <= command_count; i++)
        {
            char index[10];
            sprintf(index, "%d", i);
            write(STDOUT_FILENO, index, strlen(index));
            write(STDOUT_FILENO, "\t", strlen("\t"));
            write(STDOUT_FILENO, history[i], strlen(history[i]));
            write(STDOUT_FILENO, "\n", strlen("\n"));
        }
    }
}

/**
 * Need to combine !! and !n
*/
void internalCmds(char *tokens[], _Bool in_background)
{

    char temp[1024];
    if (tokens[0] == NULL)
    {
        return;
    }

    // if !!
    if (tokens[0][0] == '!')
    {
        // set the line will display
        //int history_line = 0;
        if (tokens[0][1] == '!')
        {
            // latest command line
            if (command_count == 0)
            {
                fprintf(stderr, "No History.\n");
                return;
            }
            else
            {
                write(STDOUT_FILENO, history[command_count], strlen(history[command_count]));
                write(STDOUT_FILENO, "\n", strlen("\n"));

                strcpy(temp, history[command_count]);

                int token_count = tokenize_command(temp, tokens);
                if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0)
                {
                    in_background = true;
                    tokens[token_count - 1] = 0;
                }
            }
        }
        else
        {
            // if !n
            int history_line = atoi(&tokens[0][1]);
            if (history_line == 0)
            { // n = invalid value
                fprintf(stderr, "No History.\n");
                return;
            }
            else
            {
                if ((history_line < command_count - 9) || (history_line > command_count) || (history_line < 0))
                {
                    fprintf(stderr, "No History.\n");
                    return;
                }
                else
                {
                    write(STDOUT_FILENO, history[history_line], strlen(history[history_line]));
                    write(STDOUT_FILENO, "\n", strlen("\n"));
                    strcpy(temp, history[history_line]);
                    int token_count = tokenize_command(temp, tokens);
                    if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0)
                    {
                        in_background = true;
                        tokens[token_count - 1] = 0;
                    }
                }
            }
        }
    }

    add_to_history(tokens, in_background);

    if (strcmp(tokens[0], "history") == 0)
    {
        show_history();
        return;
    }

    if (strcmp(tokens[0], "exit") == 0)
    {
        exit(0);
    }
    else if (strcmp(tokens[0], "pwd") == 0)
    {
        char cwd_store[1024];
        getcwd(cwd_store, sizeof(cwd_store));
        write(STDOUT_FILENO, cwd_store, strlen(cwd_store));
        write(STDOUT_FILENO, "\n", strlen("\n"));

        return;
    }

    else if (strcmp(tokens[0], "cd") == 0)
    {
        if (chdir(tokens[1]) == -1)
        {
            fprintf(stderr, "Invalid directory\n");
        }
        return;
    }
    //this is for type function
    else if (strcmp(tokens[0], "type") == 0)
    {
        if (strcmp(tokens[1], "exit") == 0 || strcmp(tokens[1], "pwd") == 0 || strcmp(tokens[1], "cd") == 0 || strcmp(tokens[1], "type") == 0)
        {
            char *str = strcat(tokens[1], " is a shell builtin command\n");
            write(STDOUT_FILENO, str, strlen(str));
            return;
        }
        else
        {
            char *str1 = strcat(tokens[1], " is external shell command\n");
            write(STDOUT_FILENO, str1, strlen(str1));
            return;
        }
    }
    else
    {
        int status;
        pid_t pid;
        pid = fork();

        if (pid < 0)
        {
            fprintf(stderr, "Fork is not success");
            exit(-1);
        }
        else if (pid == 0)
        {
            if (execvp(tokens[0], tokens) == -1)
            {
                write(STDOUT_FILENO, tokens[0], strlen(tokens[0]));
                write(STDOUT_FILENO, ": Unknown command.\n", strlen(": Unknown command.\n"));
                exit(1);
            }
            exit(0);
        }
        else if (!in_background)
        {
            waitpid(pid, &status, WUNTRACED);
        }
    }
}
void handle_SIGINT()
{
    write(STDOUT_FILENO, "\n", strlen("\n"));
    show_history();
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    write(STDOUT_FILENO, strcat(cwd, "$ "), strlen(strcat(cwd, "$ ")));
}

int main(int argc, char *argv[])
{
    struct sigaction handler;
    handler.sa_handler = handle_SIGINT;
    handler.sa_flags = 0;
    sigemptyset(&handler.sa_mask);
    sigaction(SIGINT, &handler, NULL);

    char input_buffer[COMMAND_LENGTH];
    char *tokens[NUM_TOKENS];
    while (true)
    {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        write(STDOUT_FILENO, strcat(cwd, "$ "), strlen(strcat(cwd, "$ ")));
        _Bool in_background = false;
        read_command(input_buffer, tokens, &in_background);

        internalCmds(tokens, in_background);
    }
    return 0;
}
