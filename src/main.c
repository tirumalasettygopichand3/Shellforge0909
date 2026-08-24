#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "lexer.h"
#include "parser.h"
#include "expand.h"

static void print_command(Command *command)
{
    printf("\nParsed command:\n");

    printf("Arguments:");

    for (int i = 0; i < command->argument_count; i++)
        printf(" [%s]", command->arguments[i]);

    printf("\n");

    if (command->input_file != NULL)
        printf("Input file: %s\n", command->input_file);

    if (command->output_file != NULL)
        printf("Output file: %s\n", command->output_file);

    if (command->append_file != NULL)
        printf("Append file: %s\n", command->append_file);

    printf("Background: %s\n",
           command->background ? "yes" : "no");

    printf("\n");
}

int main(void)
{
    char *input;

    printf("=================================\n");
    printf("       Welcome to Shellforge\n");
    printf("       Milestone 2.2\n");
    printf("       Parser & Expand\n");
    printf("=================================\n");

    while (1) {

        input = readline("shellforge> ");

        if (input == NULL) {
            printf("\nExiting Shellforge.\n");
            break;
        }

        if (strlen(input) == 0) {
            free(input);
            continue;
        }

        if (strcmp(input, "exit") == 0) {
            free(input);
            printf("Exiting Shellforge.\n");
            break;
        }

        add_history(input);

        int token_count = 0;

        Token **tokens = tokenize(input, &token_count);

        Command *command =
            parse_tokens(tokens, token_count);

        expand_command(command);

        print_command(command);

        free_command(command);
        free_tokens(tokens, token_count);
        free(input);
    }

    return 0;
}
