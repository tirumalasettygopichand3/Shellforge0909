#ifndef LEXER_H
#define LEXER_H

#include "token.h"

Token **tokenize(const char *input, int *token_count);
void free_tokens(Token **tokens, int token_count);

#endif
