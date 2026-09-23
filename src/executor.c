#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "executor.h"
#include "builtin.h"


static int apply_redirection(Command *command)
{
    int fd;


    /* INPUT */

    if (command->input_file != NULL)
    {
        fd = open(
            command->input_file,
            O_RDONLY
        );

        if (fd < 0)
        {
            perror(command->input_file);
            return -1;
        }

        if (dup2(fd, STDIN_FILENO) < 0)
        {
            perror("dup2");
            close(fd);
            return -1;
        }

        close(fd);
    }


    /* OUTPUT */

    if (command->output_file != NULL)
    {
        fd = open(
            command->output_file,
            O_WRONLY | O_CREAT | O_TRUNC,
            0644
        );

        if (fd < 0)
        {
            perror(command->output_file);
            return -1;
        }

        if (dup2(fd, STDOUT_FILENO) < 0)
        {
            perror("dup2");
            close(fd);
            return -1;
        }

        close(fd);
    }


    /* APPEND */

    if (command->append_file != NULL)
    {
        fd = open(
            command->append_file,
            O_WRONLY | O_CREAT | O_APPEND,
            0644
        );

        if (fd < 0)
        {
            perror(command->append_file);
            return -1;
        }

        if (dup2(fd, STDOUT_FILENO) < 0)
        {
            perror("dup2");
            close(fd);
            return -1;
        }

        close(fd);
    }


    return 0;
}


int execute_command(Command *command)
{
    if (command == NULL ||
        command->argument_count == 0)
    {
        return -1;
    }


    /* Built-in command */

    if (is_builtin(command))
    {
        return execute_builtin(command);
    }


    /* External command */

    pid_t pid = fork();


    if (pid < 0)
    {
        perror("fork");
        return -1;
    }


    /* CHILD */

    if (pid == 0)
    {
        if (apply_redirection(command) < 0)
        {
            _exit(1);
        }


        execvp(
            command->arguments[0],
            command->arguments
        );


        perror(command->arguments[0]);

        _exit(127);
    }


    /* PARENT */

    if (!command->background)
    {
        int status;

        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return -1;
        }

        if (WIFEXITED(status))
            return WEXITSTATUS(status);
    }


    return 0;
}


int execute_pipeline(Pipeline *pipeline)
{
    if (pipeline == NULL ||
        pipeline->command_count == 0)
    {
        return -1;
    }


    /* One command */

    if (pipeline->command_count == 1)
    {
        return execute_command(
            pipeline->commands[0]
        );
    }


    int command_count = pipeline->command_count;


    int (*pipes)[2] =
        malloc(
            sizeof(int[2]) *
            (command_count - 1)
        );


    if (pipes == NULL)
    {
        perror("malloc");
        return -1;
    }


    /* Create pipes */

    for (int i = 0; i < command_count - 1; i++)
    {
        if (pipe(pipes[i]) < 0)
        {
            perror("pipe");

            for (int j = 0; j < i; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            free(pipes);

            return -1;
        }
    }


    pid_t *pids =
        malloc(
            sizeof(pid_t) * command_count
        );


    if (pids == NULL)
    {
        perror("malloc");

        for (int i = 0; i < command_count - 1; i++)
        {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        free(pipes);

        return -1;
    }


    /* Create one child for each command */

    for (int i = 0; i < command_count; i++)
    {
        pids[i] = fork();


        if (pids[i] < 0)
        {
            perror("fork");
            continue;
        }


        /* CHILD */

        if (pids[i] == 0)
        {
            Command *command =
                pipeline->commands[i];


            /* Read from previous pipe */

            if (i > 0)
            {
                if (dup2(
                        pipes[i - 1][0],
                        STDIN_FILENO
                    ) < 0)
                {
                    perror("dup2");
                    _exit(1);
                }
            }


            /* Write to next pipe */

            if (i < command_count - 1)
            {
                if (dup2(
                        pipes[i][1],
                        STDOUT_FILENO
                    ) < 0)
                {
                    perror("dup2");
                    _exit(1);
                }
            }


            /* Apply command redirections */

            if (apply_redirection(command) < 0)
            {
                _exit(1);
            }


            /* Close all pipe descriptors */

            for (int j = 0;
                 j < command_count - 1;
                 j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }


            execvp(
                command->arguments[0],
                command->arguments
            );


            perror(command->arguments[0]);

            _exit(127);
        }
    }


    /* PARENT closes all pipe descriptors */

    for (int i = 0;
         i < command_count - 1;
         i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }


    /* Wait for all children */

    int last_status = 0;

    for (int i = 0;
         i < command_count;
         i++)
    {
        int status;

        if (waitpid(
                pids[i],
                &status,
                0
            ) < 0)
        {
            perror("waitpid");
            continue;
        }


        if (i == command_count - 1)
        {
            if (WIFEXITED(status))
                last_status =
                    WEXITSTATUS(status);
        }
    }


    free(pids);

    free(pipes);


    return last_status;
}
