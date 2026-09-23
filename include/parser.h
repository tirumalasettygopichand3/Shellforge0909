#ifndef PARSER_H
#define PARSER_H

#include "token.h"

typedef struct
{
    char **arguments;
    int argument_count;

    char *input_file;
    char *output_file;
    char *append_file;

    int background;

} Command;


typedef struct
{
    Command **commands;
    int command_count;

} Pipeline;


Command *parse_tokens(Token **tokens, int token_count);

Pipeline *parse_pipeline(Token **tokens, int token_count);

void free_command(Command *command);

void free_pipeline(Pipeline *pipeline);

#endif
