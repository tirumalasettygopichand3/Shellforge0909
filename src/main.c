#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "lexer.h"
#include "parser.h"
#include "builtin.h"
#include "executor.h"

int main(void)
{
    char *input;

    printf("========================================\n");
    printf("          Welcome to Shellforge\n");
    printf("       External Command Execution\n");
    printf("========================================\n");

    while (1)
    {
        input = readline("shellforge$ ");

        if (input == NULL)
        {
            printf("\n");
            break;
        }

        if (strlen(input) == 0)
        {
            free(input);
            continue;
        }

        add_history(input);

        int token_count = 0;

        Token **tokens = tokenize(input, &token_count);

        if (tokens == NULL)
        {
            free(input);
            continue;
        }

        Command *command = parse_tokens(tokens, token_count);

        if (command == NULL)
        {
            free_tokens(tokens, token_count);
            free(input);
            continue;
        }

        /*
         * First check whether the command is a built-in.
         */
        if (is_builtin(command))
        {
            execute_builtin(command);
        }
        else
        {
            /*
             * Otherwise execute it as an external command.
             */
            execute_external(command);
        }

        free_command(command);
        free_tokens(tokens, token_count);
        free(input);
    }

    return 0;
}
