#ifndef TOKEN_UTILS_H
#define TOKEN_UTILS_H

#include <stdio.h>

#include "../lexer_parser/lexer/lexer.h"
#include "../lexer_parser/lexer/token.h"

int is_separator(char c);

char *get_string(FILE *fd);

char *get_word(FILE *fd);

struct token *set_token(struct token *res, char *str);

void skip_comment(struct lexer *lexer);

int is_redir(char *str);

char *get_double_quote(FILE *fd);

#endif /*TOKEN_UTILS_H*/
