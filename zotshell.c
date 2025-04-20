#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_LINE 80  /* The maximum length command */

int main(void)
{
    char input[MAX_LINE];
    char *args[MAX_LINE/2 + 1]; /* command line arguments */

    int should_run = 1; /* flag to determine when to exit program */


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
        
        // fork the process to execute the input command
        pid = fork();

        // I got the following from the slides 
        if (pid < 0) { // if an error occurs
            fprintf(stderr, "Fork failed, terminating program"); 
            return 1;
        }
        else if (pid == 0) { // child process
            execvp(args[0], args);
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
