#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"


/* Create a copy of a string */
static char *duplicate_string(const char *str)
{
    char *copy;

    if (str == NULL)
        return NULL;

    copy = malloc(strlen(str) + 1);

    if (copy == NULL)
        return NULL;

    strcpy(copy, str);

    return copy;
}


/* Create an empty command */
static Command *create_command(void)
{
    Command *command;

    command = malloc(sizeof(Command));

    if (command == NULL)
        return NULL;

    command->arguments = NULL;
    command->argument_count = 0;
    command->input_file = NULL;
    command->output_file = NULL;
    command->append_file = NULL;
    command->background = 0;

    return command;
}


/* Add an argument to a command */
static int add_argument(Command *command, const char *argument)
{
    char **new_arguments;

    new_arguments = realloc(
        command->arguments,
        sizeof(char *) * (command->argument_count + 2)
    );

    if (new_arguments == NULL)
        return -1;

    command->arguments = new_arguments;

    command->arguments[command->argument_count] =
        duplicate_string(argument);

    if (command->arguments[command->argument_count] == NULL)
        return -1;

    command->argument_count++;

    /* execvp() requires NULL at the end */
    command->arguments[command->argument_count] = NULL;

    return 0;
}


/* --------------------------------------------------
   Parse a single command
   -------------------------------------------------- */
Command *parse_tokens(Token **tokens, int token_count)
{
    Command *command;
    int i;

    if (tokens == NULL || token_count <= 0)
        return NULL;

    command = create_command();

    if (command == NULL)
        return NULL;

    for (i = 0; i < token_count; i++)
    {
        switch (tokens[i]->type)
        {
            case TOKEN_WORD:

                if (add_argument(
                        command,
                        tokens[i]->value) < 0)
                {
                    free_command(command);
                    return NULL;
                }

                break;


            case TOKEN_INPUT:

                if (i + 1 < token_count &&
                    tokens[i + 1]->type == TOKEN_WORD)
                {
                    command->input_file =
                        duplicate_string(tokens[i + 1]->value);

                    i++;
                }

                break;


            case TOKEN_OUTPUT:

                if (i + 1 < token_count &&
                    tokens[i + 1]->type == TOKEN_WORD)
                {
                    command->output_file =
                        duplicate_string(tokens[i + 1]->value);

                    i++;
                }

                break;


            case TOKEN_APPEND:

                if (i + 1 < token_count &&
                    tokens[i + 1]->type == TOKEN_WORD)
                {
                    command->append_file =
                        duplicate_string(tokens[i + 1]->value);

                    i++;
                }

                break;


            case TOKEN_BACKGROUND:

                command->background = 1;

                break;


            default:

                break;
        }
    }

    return command;
}


/* --------------------------------------------------
   Parse a complete pipeline
   -------------------------------------------------- */
Pipeline *parse_pipeline(Token **tokens, int token_count)
{
    Pipeline *pipeline;
    Command *current;
    int i;

    if (tokens == NULL || token_count <= 0)
        return NULL;

    pipeline = malloc(sizeof(Pipeline));

    if (pipeline == NULL)
        return NULL;

    pipeline->commands = NULL;
    pipeline->command_count = 0;

    current = create_command();

    if (current == NULL)
    {
        free(pipeline);
        return NULL;
    }

    for (i = 0; i < token_count; i++)
    {
        Token *token = tokens[i];

        /* ------------------------------------------
           PIPE
           ------------------------------------------ */
        if (token->type == TOKEN_PIPE)
        {
            if (current->argument_count > 0)
            {
                Command **new_commands;

                new_commands = realloc(
                    pipeline->commands,
                    sizeof(Command *) *
                    (pipeline->command_count + 1)
                );

                if (new_commands == NULL)
                {
                    free_command(current);
                    free_pipeline(pipeline);
                    return NULL;
                }

                pipeline->commands = new_commands;

                pipeline->commands[
                    pipeline->command_count
                ] = current;

                pipeline->command_count++;

                current = create_command();

                if (current == NULL)
                {
                    free_pipeline(pipeline);
                    return NULL;
                }
            }

            continue;
        }


        /* ------------------------------------------
           WORD
           ------------------------------------------ */
        if (token->type == TOKEN_WORD)
        {
            if (add_argument(
                    current,
                    token->value) < 0)
            {
                free_command(current);
                free_pipeline(pipeline);
                return NULL;
            }

            continue;
        }


        /* ------------------------------------------
           INPUT REDIRECTION <
           ------------------------------------------ */
        if (token->type == TOKEN_INPUT)
        {
            if (i + 1 < token_count &&
                tokens[i + 1]->type == TOKEN_WORD)
            {
                free(current->input_file);

                current->input_file =
                    duplicate_string(
                        tokens[i + 1]->value
                    );

                i++;
            }

            continue;
        }


        /* ------------------------------------------
           OUTPUT REDIRECTION >
           ------------------------------------------ */
        if (token->type == TOKEN_OUTPUT)
        {
            if (i + 1 < token_count &&
                tokens[i + 1]->type == TOKEN_WORD)
            {
                free(current->output_file);

                current->output_file =
                    duplicate_string(
                        tokens[i + 1]->value
                    );

                i++;
            }

            continue;
        }


        /* ------------------------------------------
           APPEND REDIRECTION >>
           ------------------------------------------ */
        if (token->type == TOKEN_APPEND)
        {
            if (i + 1 < token_count &&
                tokens[i + 1]->type == TOKEN_WORD)
            {
                free(current->append_file);

                current->append_file =
                    duplicate_string(
                        tokens[i + 1]->value
                    );

                i++;
            }

            continue;
        }


        /* ------------------------------------------
           BACKGROUND &
           ------------------------------------------ */
        if (token->type == TOKEN_BACKGROUND)
        {
            current->background = 1;
        }
    }


    /* Add final command */
    if (current->argument_count > 0)
    {
        Command **new_commands;

        new_commands = realloc(
            pipeline->commands,
            sizeof(Command *) *
            (pipeline->command_count + 1)
        );

        if (new_commands == NULL)
        {
            free_command(current);
            free_pipeline(pipeline);
            return NULL;
        }

        pipeline->commands = new_commands;

        pipeline->commands[
            pipeline->command_count
        ] = current;

        pipeline->command_count++;
    }
    else
    {
        free_command(current);
    }

    return pipeline;
}


/* --------------------------------------------------
   Free a Command
   -------------------------------------------------- */
void free_command(Command *command)
{
    int i;

    if (command == NULL)
        return;

    for (i = 0; i < command->argument_count; i++)
    {
        free(command->arguments[i]);
    }

    free(command->arguments);

    free(command->input_file);
    free(command->output_file);
    free(command->append_file);

    free(command);
}


/* --------------------------------------------------
   Free a Pipeline
   -------------------------------------------------- */
void free_pipeline(Pipeline *pipeline)
{
    int i;

    if (pipeline == NULL)
        return;

    for (i = 0; i < pipeline->command_count; i++)
    {
        free_command(pipeline->commands[i]);
    }

    free(pipeline->commands);

    free(pipeline);
}
