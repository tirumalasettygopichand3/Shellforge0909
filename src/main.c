#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "lexer.h"
#include "parser.h"
#include "expand.h"
#include "executor.h"


/* Print token information */
void print_tokens(Token **tokens, int token_count)
{
    printf("\n");
    printf("========== TOKENS ==========\n\n");

    for (int i = 0; i < token_count; i++)
    {
        printf("%d : ", i);

        switch (tokens[i]->type)
        {
            case TOKEN_WORD:
                printf("WORD      ");
                break;

            case TOKEN_PIPE:
                printf("PIPE      ");
                break;

            case TOKEN_INPUT:
                printf("INPUT     ");
                break;

            case TOKEN_OUTPUT:
                printf("OUTPUT    ");
                break;

            case TOKEN_APPEND:
                printf("APPEND    ");
                break;

            case TOKEN_BACKGROUND:
                printf("BACKGROUND ");
                break;

            case TOKEN_SEMICOLON:
                printf("SEMICOLON ");
                break;

            case TOKEN_EOF:
                printf("END       ");
                break;

            default:
                printf("UNKNOWN   ");
                break;
        }

        if (tokens[i]->value != NULL)
            printf("%s", tokens[i]->value);
        else
            printf("END");

        printf("\n");
    }

    printf("============================\n");
}


/* Print pipeline information */
void print_pipeline(Pipeline *pipeline)
{
    printf("\n");
    printf("========== PIPELINE ==========\n");

    for (int i = 0; i < pipeline->command_count; i++)
    {
        Command *cmd = pipeline->commands[i];

        printf("\n");
        printf("Command %d\n", i + 1);
        printf("------------------------------\n");

        printf("Arguments\n");

        for (int j = 0; j < cmd->argument_count; j++)
        {
            printf("argv[%d] = %s\n",
                   j,
                   cmd->arguments[j]);
        }

        if (cmd->input_file != NULL)
            printf("Input     : %s\n", cmd->input_file);
        else
            printf("Input     : None\n");

        if (cmd->output_file != NULL)
            printf("Output    : %s\n", cmd->output_file);
        else
            printf("Output    : None\n");

        if (cmd->append_file != NULL)
            printf("Append    : %s\n", cmd->append_file);
        else
            printf("Append    : No\n");

        printf("Background: %s\n",
               cmd->background ? "Yes" : "No");
    }

    printf("==============================\n");
}


int main(void)
{
    char *input;

    printf("========================================\n");
    printf("          Welcome to Shellforge\n");
    printf("       Executor & Pipelines\n");
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

        if (strcmp(input, "exit") == 0)
        {
            free(input);
            break;
        }

        int token_count = 0;

        Token **tokens = tokenize(
            input,
            &token_count
        );

        if (tokens == NULL)
        {
            free(input);
            continue;
        }

        /* Display tokens */
        print_tokens(tokens, token_count);

        Pipeline *pipeline = parse_pipeline(
            tokens,
            token_count
        );

        if (pipeline == NULL)
        {
            free_tokens(
                tokens,
                token_count
            );

            free(input);
            continue;
        }

        /* Apply wildcard expansion */
        for (int i = 0;
             i < pipeline->command_count;
             i++)
        {
            expand_command(
                pipeline->commands[i]
            );
        }

        /* Display pipeline */
        print_pipeline(pipeline);

        /* Execute command / pipeline */
        execute_pipeline(pipeline);

        free_pipeline(pipeline);

        free_tokens(
            tokens,
            token_count
        );

        free(input);
    }

    return 0;
}
