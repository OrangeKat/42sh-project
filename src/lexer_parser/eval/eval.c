#include "eval.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../../builtins/builtin.h"
#include "../../utils/variable.h"

static char **expand_vars(char **data, size_t size)
{
    char **res = malloc(sizeof(char *) * (size + 1));
    for (size_t i = 0; i < size; i++)
    {
        if (*data[i] == '$')
        {
            extern struct var_holder *g_vh;
            struct var *v = get_variable(data[i] + 1, g_vh);
            if (v->type == STRING)
            {
                res[i] = v->data.string;
            }
        }
        else
        {
            res[i] = data[i];
        }
    }
    res[size] = NULL;
    return res;
}

static void execute_cmd(char **data, int *ret_val)
{
    pid_t child = fork();
    if (child == 0)
    {
        execvp(data[0], data);
        exit(127);
    }
    else
    {
        int status = 0;
        waitpid(child, &status, 0);
        if (WIFEXITED(status))
        {
            *ret_val = WEXITSTATUS(status);
        }
    }
}

int cmd_handler(char **data, size_t size)
{
    char **data_exp = expand_vars(data, size);
    if (strcmp(data[0], "true") == 0)
    {
        int res = bin_true();
        fflush(stdout);
        free(data_exp);
        return res;
    }
    else if (strcmp(data[0], "false") == 0)
    {
        int res = bin_false();
        fflush(stdout);
        free(data_exp);
        return res;
    }
    else if (strcmp(data[0], "echo") == 0)
    {
        int res = !echo(data_exp, size);
        fflush(stdout);
        free(data_exp);
        return res;
    }
    else if (strcmp(data[0], "exit") == 0)
    {
        int res = bin_exit(data_exp, size);
        fflush(stdout);
        free(data_exp);
        return res;
    }
    else if (strcmp(data[0], "cd") == 0)
    {
        int res = cd(data_exp, size);
        fflush(stdout);
        free(data_exp);
        return res;
    }
    else
    {
        int status = 0;
        execute_cmd(data_exp, &status);
        free(data_exp);
        return status == 127 ? -1 : 1;
    }
}

static int ast_eval_cmd(struct ast *ast)
{
    if (ast->nb_children == 1)
    {
        return select_redir(ast);
    }
    size_t size = 0;
    while (ast->data[size] != NULL)
    {
        size++;
    }
    return cmd_handler(ast->data, size);
}

static int ast_eval_list(struct ast *ast)
{
    int ret_val = 1;
    for (size_t i = 0; i < ast->nb_children; i++)
    {
        ret_val = ast_eval(ast->children[i]);
        if (ret_val == -1)
        {
            return 127;
        }
        if (!ret_val)
        {
            ret_val = 0;
        }
    }
    return ret_val;
}

static int ast_eval_if(struct ast *ast)
{
    if (ast_eval(ast->children[0]) > 0)
    {
        return ast_eval(ast->children[1]);
    }
    else
    {
        if (ast->nb_children == 3)
        {
            return ast_eval(ast->children[2]);
        }
        return 1;
    }
}

static int ast_eval_pipe(struct ast *ast)
{
    if (ast->nb_children == 1)
    {
        return ast_eval(ast->children[0]);
    }
    else
    {
        return run_piped_commands(ast);
    }
}

static int ast_eval_not(struct ast *ast)
{
    return !ast_eval(ast->children[0]);
}

static int ast_eval_while(struct ast *ast)
{
    int ret = 1;
    while (ast_eval(ast->children[0]) > 0)
    {
        ret = ast_eval(ast->children[1]);
    }
    return ret;
}

static int ast_eval_until(struct ast *ast)
{
    int ret = 1;
    while ((ast_eval(ast->children[0]) == 0))
    {
        ret = ast_eval(ast->children[1]);
    }
    return ret;
}

static int ast_eval_and(struct ast *ast)
{
    int ret;
    if ((ret = ast_eval(ast->children[0]) > 0))
    {
        return ast_eval(ast->children[1]);
    }
    return ret;
}

static int ast_eval_or(struct ast *ast)
{
    int ret;
    if (!(ret = ast_eval(ast->children[0]) > 0))
    {
        ret = ast_eval(ast->children[1]);
    }
    return ret;
}

int ast_eval(struct ast *ast)
{
    if (!ast)
    {
        return 1;
    }

    switch (ast->type)
    {
    case AST_CMD:
        return ast_eval_cmd(ast);
    case AST_LIST:
        return ast_eval_list(ast);
    case AST_IF:
        return ast_eval_if(ast);
    case AST_PIPE:
        return ast_eval_pipe(ast);
    case AST_NOT:
        return ast_eval_not(ast);
    case AST_WHILE:
        return ast_eval_while(ast);
    case AST_UNTIL:
        return ast_eval_until(ast);
    case AST_AND:
        return ast_eval_and(ast);
    case AST_OR:
        return ast_eval_or(ast);
    default:
        return 1;
    }
}
