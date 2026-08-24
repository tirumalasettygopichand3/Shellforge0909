#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

static char *duplicate_string(const char *text)
{
    if (text == NULL)
        return NULL;

    char *copy = malloc(strlen(text) + 1);

    if (copy == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(copy, text);
    return copy;
}

Command *parse_tokens(Token **tokens, int token_count)
{
    Command *command = malloc(sizeof(Command));

    if (command == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    command->arguments = malloc(sizeof(char *) * (token_count + 1));

    if (command->arguments == NULL) {
        perror("malloc");
        free(command);
        exit(EXIT_FAILURE);
    }

    command->argument_count = 0;
    command->input_file = NULL;
    command->output_file = NULL;
    command->append_file = NULL;
    command->background = 0;

    for (int i = 0; i < token_count; i++) {

        if (tokens[i]->type == TOKEN_WORD) {
            command->arguments[command->argument_count] =
                duplicate_string(tokens[i]->value);

            command->argument_count++;
        }

        else if (tokens[i]->type == TOKEN_INPUT) {
            if (i + 1 < token_count &&
                tokens[i + 1]->type == TOKEN_WORD) {

                command->input_file =
                    duplicate_string(tokens[++i]->value);
            }
        }

        else if (tokens[i]->type == TOKEN_OUTPUT) {
            if (i + 1 < token_count &&
                tokens[i + 1]->type == TOKEN_WORD) {

                command->output_file =
                    duplicate_string(tokens[++i]->value);
            }
        }

        else if (tokens[i]->type == TOKEN_APPEND) {
            if (i + 1 < token_count &&
                tokens[i + 1]->type == TOKEN_WORD) {

                command->append_file =
                    duplicate_string(tokens[++i]->value);
            }
        }

        else if (tokens[i]->type == TOKEN_BACKGROUND) {
            command->background = 1;
        }
    }

    command->arguments[command->argument_count] = NULL;

    return command;
}

void free_command(Command *command)
{
    if (command == NULL)
        return;

    for (int i = 0; i < command->argument_count; i++)
        free(command->arguments[i]);

    free(command->arguments);
    free(command->input_file);
    free(command->output_file);
    free(command->append_file);
    free(command);
}
