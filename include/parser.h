#ifndef PARSER_H
#define PARSER_H

#include "token.h"

typedef struct {
    char **arguments;
    int argument_count;
    char *input_file;
    char *output_file;
    char *append_file;
    int background;
} Command;

Command *parse_tokens(Token **tokens, int token_count);
void free_command(Command *command);

#endif
