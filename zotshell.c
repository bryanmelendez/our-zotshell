#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE 80  /* The maximum length command */

int main(void)
{
    char input[MAX_LINE];
    char *command;
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    int arg_num = 0;

    int should_run = 1; /* flag to determine when to exit program */

    while (should_run) {
        printf("osh>");

        scanf("%[^\n]s", input);

        fork();

        command = strtok(input, " ");
        args[arg_num] = strtok(NULL, " ");

        while (args[arg_num] != NULL) {
            arg_num++;
            args[arg_num] = strtok(NULL, " ");
        }
        
        execvp(command, args);

        fflush(stdout);

        /**
        * After reading user input, the steps are:
        * (1) fork a child process using fork()
        * (2) the child process will invoke execvp()
        * (3) parent will invoke wait() unless command included &
        */
    }

    return 0;
}