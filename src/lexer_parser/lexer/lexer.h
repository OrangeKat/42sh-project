#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#include "token.h"

struct lexer
{
    FILE *input_file; // The input data
    size_t offset; // The current offset inside the input data
    struct token *current_tok; // The next token, if processed
    int separator;
};

struct lexer *lexer_genesis(FILE *input_file);

void lexer_destroy(struct lexer *lexer);

struct token *parse_input_for_tok(struct lexer *lexer);

struct token *lexer_peek(struct lexer *lexer);

struct token *lexer_pop(struct lexer *lexer);

#endif /* !LEXER_H */
