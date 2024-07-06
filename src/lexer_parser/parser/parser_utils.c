#include "parser_utils.h"

#include <stdlib.h>

#include "../lexer/lexer.h"
#include "../lexer/token.h"

void newline_cleanser(struct lexer *lexer)
{
    while (lexer->current_tok->type == TOKEN_NL)
    {
        free(lexer_pop(lexer));
    }
}

char **add_to_data(char **data, char *word)
{
    size_t length = 0;
    while (data[length] != NULL)
    {
        length++;
    }
    data[length] = word;
    length++;

    data = realloc(data, (length + 1) * sizeof(char *));
    data[length] = NULL;
    return data;
}

void free_token(struct token *token)
{
    if (token->value)
    {
        free(token->value);
    }
    free(token);
}

int list_node_continue(enum token_type type)
{
    switch (type)
    {
    case TOKEN_EOF:
        return 0;
    case TOKEN_THEN:
        return 0;
    case TOKEN_DO:
        return 0;
    case TOKEN_FI:
        return 0;
    case TOKEN_DONE:
        return 0;
    case TOKEN_ELSE:
        return 0;
    case TOKEN_ELIF:
        return 0;
    default:
        return 1;
    }
}

int exp_node_continue(enum token_type type)
{
    switch (type)
    {
    case TOKEN_NL:
        return 0;
    case TOKEN_EOF:
        return 0;
    case TOKEN_PIPE:
        return 0;
    case TOKEN_AND:
        return 0;
    case TOKEN_OR:
        return 0;
    case TOKEN_REDIR:
        return 0;
    case TOKEN_ERROR:
        return 0;
    default:
        return 1;
    }
}

int is_shell_command(enum token_type type)
{
    switch (type)
    {
    case TOKEN_IF:
        return 1;
    case TOKEN_WHILE:
        return 1;
    case TOKEN_UNTIL:
        return 1;
    default:
        return 0;
    }
}
