#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "lexer.h"
#include "token.h"

int main(void)
{
    char *input;

    printf("=================================\n");
    printf("       Welcome to Shellforge\n");
    printf("       Milestone 2 - Lexer\n");
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

        printf("\nTokens:\n");

        for (int i = 0; i < token_count; i++) {
            printf("  [%d] %-12s : %s\n",
                   i,
                   token_type_to_string(tokens[i]->type),
                   tokens[i]->value);
        }

        printf("\n");

        free_tokens(tokens, token_count);
        free(input);
    }

    return 0;
}
