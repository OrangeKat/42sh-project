#ifndef TOKEN_H
#define TOKEN_H

#include <unistd.h>

enum token_type
{
    TOKEN_WORD,
    TOKEN_EOF,
    TOKEN_NL,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_FI,
    TOKEN_ELIF,
    TOKEN_PIPE,
    TOKEN_NOT,
    TOKEN_REDIR,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_UNTIL,
    TOKEN_DO,
    TOKEN_DONE,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_VAR,
    TOKEN_ERROR,
    TOKEN_THEN
};

struct token
{
    enum token_type type; // The kind of token
    char *value; // If the token is a word, its value
};
#endif /* !TOKEN_H */
