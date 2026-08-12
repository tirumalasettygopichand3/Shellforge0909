#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

Token *create_token(TokenType type, const char *value)
{
    Token *token = malloc(sizeof(Token));

    if (token == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    token->type = type;

    if (value != NULL) {
        token->value = malloc(strlen(value) + 1);

        if (token->value == NULL) {
            perror("malloc");
            free(token);
            exit(EXIT_FAILURE);
        }

        strcpy(token->value, value);
    } else {
        token->value = NULL;
    }

    return token;
}

void free_token(Token *token)
{
    if (token == NULL)
        return;

    free(token->value);
    free(token);
}

const char *token_type_to_string(TokenType type)
{
    switch (type) {
        case TOKEN_WORD:
            return "WORD";

        case TOKEN_PIPE:
            return "PIPE";

        case TOKEN_INPUT:
            return "INPUT";

        case TOKEN_OUTPUT:
            return "OUTPUT";

        case TOKEN_APPEND:
            return "APPEND";

        case TOKEN_BACKGROUND:
            return "BACKGROUND";

        case TOKEN_SEMICOLON:
            return "SEMICOLON";

        case TOKEN_EOF:
            return "EOF";

        default:
            return "UNKNOWN";
    }
}
