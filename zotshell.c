#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_LINE 80  /* The maximum length command */

int main(void)
{
    char input[MAX_LINE];
    char *args[MAX_LINE/2 + 1]; /* command line arguments */

    int should_run = 1; /* flag to determine when to exit program */
    char *history[MAX_LINE/2 + 1];


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
            printf("want history!\n");
            printf("last cmd: %s\n", history[0]);
        }
        else{
            // If the command is not exit
            // fork the process to execute the input command
            // TODO: Issue is that !! is still added to the history
            if(strcmp(args[0], "!!") != 0)
            {
                memcpy(history, args, MAX_LINE/2 + 1);
                printf("Added :%s to history!\n", history[0]);
            }

            pid = fork();
        }

        // I got the following from the slides 
        if (pid < 0) { // if an error occurs
            fprintf(stderr, "Fork failed, terminating program"); 
            return 1;
        }
        else if (pid == 0) { // child process
            
            execvp(args[0], args);
            //memcpy(history, args, MAX_LINE/2 + 1); 
                
            
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
