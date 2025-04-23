#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_LINE 80  /* The maximum length command */

int main(void)
{
    char input[MAX_LINE];
    char *args[MAX_LINE/2 + 1] = {NULL}; /* command line arguments */

    int should_run = 1; /* flag to determine when to exit program */
    char *history[MAX_LINE/2 + 1] = {NULL};


    while (should_run) {
        int arg_num = 0;
        pid_t pid;

        printf("osh>");

        scanf("%[^\n]", input);
        getchar(); // consumes the newline

        args[arg_num] = strtok(input, " ");

        while (args[arg_num] != NULL) {
            arg_num++;
            args[arg_num] = strtok(NULL, " ");
        }
        args[++arg_num] = NULL; // terminates the argument list
        
        // If statements to handle the exit condition
        if(strcmp(args[0], "exit") == 0)
            // If the command is exit flag should_run and not fork
            should_run = 0;
        else if(strcmp(args[0], "!!") == 0)
        {
            if (history[0] == NULL)
                printf("No commands in history\n");
            else 
            {
                int k = 0;
                while(history[k] != NULL)
                {
                    printf("%s ", history[k]);
                    k++;
                }
                printf("\n");
                memcpy(args, history, MAX_LINE/2 + 1);
                pid = fork();
            }
        }
        else {
            // Deep copying args to history so it doesn't get overwritten the next iteration
            for (int i = 0; i < MAX_LINE/2 + 1; i++) 
            {
                if (history[i] != NULL)
                    free(history[i]); // Free previously allocated memory
                if (args[i] != NULL)
                    history[i] = strdup(args[i]); // Duplicate the string
                else
                    history[i] = NULL; // Ensure null-termination
            }
            //printf("Added: %s to history!\n", history[0]);
            pid = fork();
        }

        // I got the following from the slides 
        if (pid < 0) { // if an error occurs
            fprintf(stderr, "Fork failed, terminating program"); 
            return 1;
        }
        else if (pid == 0) { // child process
            // determine whether or not we need to read or write from a file
            if(strcmp(args[1], ">") == 0)
            {
                // WRITE OUTPUT OF EXECVP TO A FILE:
                int fdOut = open("output.txt", O_WRONLY, 0666);

                // then call execvp??
            }
            else if(strcmp(args[1], "<") == 0)
            {
                // HERE WE NEED TO READ THE ARGS FROM A FILE

                // then call execvp??
            }
            else
            {
                // If we don't need to write to a file, or read from a file just execute normally
                execvp(args[0], args);
            }

            // EXECVP used to be here     
            
            if (errno == ENOENT) {
                fprintf(stderr, "%s: command not found\n", args[0]);
                exit(1);
            } else {
                fprintf(stderr, "%s: %s\n", args[0], strerror(errno));
                exit(1);
            }
        }
        else { // parent process
            wait(NULL);
            // we only need to wait if user does not put &
            // in the command 
        }

        // flush input and output just in case something weird is still in there
        fflush(stdout);
        fflush(stdin);

        // zero out input array
        memset(input, '\0', sizeof(input));

        /**
        * After reading user input, the steps are:
        * (1) fork a child process using fork()
        * (2) the child process will invoke execvp()
        * (3) parent will invoke wait() unless command included &
        */
    }

    return 0;
}
