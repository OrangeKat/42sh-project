#ifndef EVAL_H
#define EVAL_H

#include "../ast/ast.h"

int ast_eval(struct ast *ast);
int run_piped_commands(struct ast *pipe_node);
int cmd_handler(char **data, size_t size);
int select_redir(struct ast *node);

#endif // EVAL_H
