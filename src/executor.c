#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "executor.h"

int execute_external(Command *command)
{
    if (command == NULL ||
        command->argument_count == 0 ||
        command->arguments == NULL)
    {
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        /*
         * Child process.
         * Replace child with the external command.
         */
        execvp(command->arguments[0], command->arguments);

        /*
         * execvp() only returns if there is an error.
         */
        perror("execvp");
        exit(1);
    }

    /*
     * Parent process waits for child.
     */
    int status;

    if (waitpid(pid, &status, 0) < 0)
    {
        perror("waitpid");
        return 1;
    }

    return 1;
}
