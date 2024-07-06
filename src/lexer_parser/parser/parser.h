#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"

enum parser_status
{
    PARSER_OK,
    PARSER_NOK,
};

enum parser_status parse(struct ast **root, struct lexer *lexer);
enum parser_status parse_else(struct lexer *lexer, struct ast **node);
enum parser_status parse_if(struct lexer *lexer, struct ast **node);
enum parser_status parse_and_or(struct lexer *lexer, struct ast **node);

#endif /* !PARSER_H */
