#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h> //file open

#define MAX_LINE 80  /* The maximum length command */

int main(void)
{
    char input[MAX_LINE];
    char *args[MAX_LINE/2 + 1] = {NULL}; /* command line arguments */

    int should_run = 1; /* flag to determine when to exit program */
    char *history[MAX_LINE/2 + 1] = {NULL};


    while (should_run) {
        int arg_num = 0;
        int fork_flag = 1; //initialize to fork
        pid_t pid = 0;

        printf("osh>");

        scanf("%[^\n]", input); // scans up until the newline
        getchar(); // consumes the newline
        
        // check if nothing was inputted
        if (input[0] == '\006' || input[0] == '\000') {
            fflush(stdout);
            fflush(stdin);
            memset(input, '\0', sizeof(input));
            continue;
        }

        args[arg_num] = strtok(input, " ");

        while (args[arg_num] != NULL) {
            arg_num++;
            args[arg_num] = strtok(NULL, " ");
        }
        args[++arg_num] = NULL; // terminates the argument list

        // If statements to handle the exit condition
        if(strcmp(args[0], "exit") == 0) {
            // If the command is exit flag should_run and not fork
            should_run = 0;
            fork_flag = 0;
        }
        else if(strcmp(args[0], "!!") == 0)
        {
            if (history[0] == NULL) {
                printf("No commands in history\n");
                fork_flag = 0;
            }
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
        }

        int redirection = 0;
        char *file;

        //loop through arguments to look for > or < operator
        for (int i = 1; i<=arg_num; i++) {
            //check for output redirection
            if(args[i] != NULL && strcmp(args[i], ">") == 0) {
                redirection = 1; //int 1 signifies output redirection
                file = args[i+1]; //saves next argument as file name
                args[i] = NULL;
            }
            //check for input redirection
            else if(args[i] != NULL && strcmp(args[i], "<") == 0) {
                redirection = 2; //int 2 signifies input redirection
                file = args[i+1]; //saves next argument as file name
                args[i] = NULL;
            }
        }

        if (fork_flag) {
            pid = fork();
        }

        // I got the following from the slides
        if (pid < 0) { // if an error occurs
            fprintf(stderr, "Fork failed, terminating program");
            return 1;
        }
        else if (pid == 0) { // child process

        if (redirection==1) {
            int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644); //open or create file
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (redirection==2) {
            int fd = open(file, O_RDONLY); //open or create file
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

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
