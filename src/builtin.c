#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "builtin.h"

int is_builtin(Command *command)
{
    if (command == NULL ||
        command->argument_count == 0 ||
        command->arguments == NULL)
    {
        return 0;
    }

    if (strcmp(command->arguments[0], "cd") == 0)
        return 1;

    if (strcmp(command->arguments[0], "pwd") == 0)
        return 1;

    if (strcmp(command->arguments[0], "echo") == 0)
        return 1;

    if (strcmp(command->arguments[0], "exit") == 0)
        return 1;

    return 0;
}

int execute_builtin(Command *command)
{
    if (!is_builtin(command))
        return 0;

    char *name = command->arguments[0];

    /* =========================
       CD COMMAND
       ========================= */
    if (strcmp(name, "cd") == 0)
    {
        char *directory;

        if (command->argument_count == 1)
        {
            directory = getenv("HOME");

            if (directory == NULL)
            {
                fprintf(stderr, "cd: HOME not set\n");
                return 1;
            }
        }
        else if (command->argument_count == 2)
        {
            directory = command->arguments[1];
        }
        else
        {
            fprintf(stderr, "cd: too many arguments\n");
            return 1;
        }

        if (chdir(directory) != 0)
        {
            perror("cd");
            return 1;
        }

        return 1;
    }

    /* =========================
       PWD COMMAND
       ========================= */
    if (strcmp(name, "pwd") == 0)
    {
        if (command->argument_count > 1)
        {
            fprintf(stderr, "pwd: too many arguments\n");
            return 1;
        }

        char cwd[4096];

        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("pwd");
            return 1;
        }

        printf("%s\n", cwd);

        return 1;
    }

    /* =========================
       ECHO COMMAND
       ========================= */
    if (strcmp(name, "echo") == 0)
    {
        for (int i = 1; i < command->argument_count; i++)
        {
            printf("%s", command->arguments[i]);

            if (i < command->argument_count - 1)
                printf(" ");
        }

        printf("\n");

        return 1;
    }

    /* =========================
       EXIT COMMAND
       ========================= */
    if (strcmp(name, "exit") == 0)
    {
        if (command->argument_count > 1)
        {
            fprintf(stderr, "exit: too many arguments\n");
            return 1;
        }

        exit(0);
    }

    return 0;
}
