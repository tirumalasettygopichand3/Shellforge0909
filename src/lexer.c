#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"

static void add_token(Token **tokens,
                      int *count,
                      int *capacity,
                      TokenType type,
                      const char *value)
{
    if (*count >= *capacity) {
        *capacity *= 2;

        Token **temp = realloc(
            tokens,
            sizeof(Token *) * (*capacity)
        );

        if (temp == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
    }

    tokens[*count] = create_token(type, value);
    (*count)++;
}

Token **tokenize(const char *input, int *token_count)
{
    int capacity = 16;
    int count = 0;
    int i = 0;

    Token **tokens = malloc(sizeof(Token *) * capacity);

    if (tokens == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (input[i] != '\0') {

        if (isspace((unsigned char)input[i])) {
            i++;
            continue;
        }

        if (input[i] == '|') {
            add_token(tokens, &count, &capacity,
                      TOKEN_PIPE, "|");
            i++;
            continue;
        }

        if (input[i] == '<') {
            add_token(tokens, &count, &capacity,
                      TOKEN_INPUT, "<");
            i++;
            continue;
        }

        if (input[i] == '>') {
            if (input[i + 1] == '>') {
                add_token(tokens, &count, &capacity,
                          TOKEN_APPEND, ">>");
                i += 2;
            } else {
                add_token(tokens, &count, &capacity,
                          TOKEN_OUTPUT, ">");
                i++;
            }
            continue;
        }

        if (input[i] == '&') {
            add_token(tokens, &count, &capacity,
                      TOKEN_BACKGROUND, "&");
            i++;
            continue;
        }

        if (input[i] == ';') {
            add_token(tokens, &count, &capacity,
                      TOKEN_SEMICOLON, ";");
            i++;
            continue;
        }

        char buffer[1024];
        int j = 0;

        while (input[i] != '\0' &&
               !isspace((unsigned char)input[i]) &&
               input[i] != '|' &&
               input[i] != '<' &&
               input[i] != '>' &&
               input[i] != '&' &&
               input[i] != ';') {

            if (input[i] == '\'') {
                i++;

                while (input[i] != '\0' &&
                       input[i] != '\'') {

                    if (j < 1023)
                        buffer[j++] = input[i];

                    i++;
                }

                if (input[i] == '\'')
                    i++;

                continue;
            }

            if (input[i] == '"') {
                i++;

                while (input[i] != '\0' &&
                       input[i] != '"') {

                    if (j < 1023)
                        buffer[j++] = input[i];

                    i++;
                }

                if (input[i] == '"')
                    i++;

                continue;
            }

            if (j < 1023)
                buffer[j++] = input[i];

            i++;
        }

        buffer[j] = '\0';

        if (j > 0) {
            add_token(tokens, &count, &capacity,
                      TOKEN_WORD, buffer);
        }
    }

    *token_count = count;

    return tokens;
}

void free_tokens(Token **tokens, int token_count)
{
    if (tokens == NULL)
        return;

    for (int i = 0; i < token_count; i++)
        free_token(tokens[i]);

    free(tokens);
}
