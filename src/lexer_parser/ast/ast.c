#include "ast.h"

#include <err.h>
#include <stdlib.h>

struct ast *ast_genesis(enum ast_type type)
{
    struct ast *new = malloc(sizeof(struct ast));
    new->type = type;
    new->nb_children = 0;
    new->children = NULL;
    new->data = malloc(sizeof(char *));
    new->data[0] = NULL;

    return new;
}

struct ast *add_child_to_parent(struct ast *parent, struct ast *child)
{
    if (parent->nb_children == 0)
    {
        parent->children = malloc(sizeof(struct ast *));
    }
    else
    {
        struct ast **tmp =
            malloc(sizeof(struct ast *) * (parent->nb_children + 1));
        for (size_t i = 0; i < parent->nb_children; i++)
        {
            tmp[i] = parent->children[i];
        }
        free(parent->children);
        parent->children = tmp;
    }
    parent->children[parent->nb_children] = child;
    parent->nb_children++;
    return parent;
}

void ast_destroy(struct ast *ast)
{
    if (!ast)
    {
        return;
    }

    for (size_t i = 0; i < ast->nb_children; i++)
    {
        ast_destroy(ast->children[i]);
    }
    free(ast->children);

    if (ast->data)
    {
        for (size_t i = 0; ast->data[i] != NULL; i++)
        {
            free(ast->data[i]);
        }
        free(ast->data);
    }

    free(ast);
}
