#ifndef AST_H
#define AST_H

#include <unistd.h>

enum ast_type
{
    AST_CMD,
    AST_NOT,
    AST_IF,
    AST_WHILE,
    AST_UNTIL,
    AST_PIPE,
    AST_REDIR,
    AST_AND,
    AST_OR,
    AST_LIST
};

struct ast
{
    enum ast_type type; // type of the node
    size_t nb_children; // size of children
    struct ast **children; // array of children
    char **data; // data contained in the node
};

void ast_destroy(struct ast *ast);
struct ast *ast_genesis(enum ast_type type);
struct ast *add_child_to_parent(struct ast *parent, struct ast *child);

#endif /* !AST_H */
