#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

#include "../lexer/lexer.h"
#include "../lexer/token.h"

char **add_to_data(char **data, char *word);
void newline_cleanser(struct lexer *lexer);
void free_token(struct token *token);
int list_node_continue(enum token_type type);
int exp_node_continue(enum token_type type);
int is_shell_command(enum token_type type);

#endif // ! PARSER_UTILS_H
