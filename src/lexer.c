#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"
#include "token.h"


static int is_special_char(char c)
{
    return c == '|' ||
           c == '<' ||
           c == '>' ||
           c == '&' ||
           c == ';';
}


Token **tokenize(const char *input, int *token_count)
{
    Token **tokens;
    int capacity = 16;
    int count = 0;
    int i = 0;

    tokens = malloc(sizeof(Token *) * capacity);

    if (tokens == NULL)
        return NULL;

    while (input[i] != '\0')
    {
        /* Skip spaces */
        if (isspace((unsigned char)input[i]))
        {
            i++;
            continue;
        }

        /* Expand token array if necessary */
        if (count >= capacity - 2)
        {
            capacity *= 2;

            tokens = realloc(
                tokens,
                sizeof(Token *) * capacity
            );

            if (tokens == NULL)
                return NULL;
        }


        /* -----------------------------
           PIPE |
           ----------------------------- */
        if (input[i] == '|')
        {
            tokens[count++] =
                create_token(TOKEN_PIPE, "|");

            i++;
            continue;
        }


        /* -----------------------------
           INPUT <
           ----------------------------- */
        if (input[i] == '<')
        {
            tokens[count++] =
                create_token(TOKEN_INPUT, "<");

            i++;
            continue;
        }


        /* -----------------------------
           OUTPUT > or APPEND >>
           ----------------------------- */
        if (input[i] == '>')
        {
            if (input[i + 1] == '>')
            {
                tokens[count++] =
                    create_token(TOKEN_APPEND, ">>");

                i += 2;
            }
            else
            {
                tokens[count++] =
                    create_token(TOKEN_OUTPUT, ">");

                i++;
            }

            continue;
        }


        /* -----------------------------
           BACKGROUND &
           ----------------------------- */
        if (input[i] == '&')
        {
            tokens[count++] =
                create_token(TOKEN_BACKGROUND, "&");

            i++;
            continue;
        }


        /* -----------------------------
           SEMICOLON ;
           ----------------------------- */
        if (input[i] == ';')
        {
            tokens[count++] =
                create_token(TOKEN_SEMICOLON, ";");

            i++;
            continue;
        }


        /* -----------------------------
           WORD
           ----------------------------- */

        {
            char buffer[1024];
            int length = 0;
            char quote = '\0';

            while (input[i] != '\0')
            {
                /* Inside quotes */
                if (quote != '\0')
                {
                    if (input[i] == quote)
                    {
                        quote = '\0';
                        i++;
                        continue;
                    }

                    if (length < 1023)
                        buffer[length++] = input[i];

                    i++;
                    continue;
                }


                /* Start quote */
                if (input[i] == '"' ||
                    input[i] == '\'')
                {
                    quote = input[i];
                    i++;
                    continue;
                }


                /* End of word */
                if (isspace((unsigned char)input[i]) ||
                    is_special_char(input[i]))
                {
                    break;
                }


                if (length < 1023)
                    buffer[length++] = input[i];

                i++;
            }

            buffer[length] = '\0';

            if (length > 0)
            {
                tokens[count++] =
                    create_token(TOKEN_WORD, buffer);
            }
        }
    }


    /* EOF token */
    tokens[count++] =
        create_token(TOKEN_EOF, NULL);

    *token_count = count;

    return tokens;
}


void free_tokens(Token **tokens, int token_count)
{
    int i;

    if (tokens == NULL)
        return;

    for (i = 0; i < token_count; i++)
    {
        free_token(tokens[i]);
    }

    free(tokens);
}
