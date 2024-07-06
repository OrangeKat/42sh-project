#include "parser.h"

#include <stdlib.h>

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "parser_utils.h"

/*
    element = WORD ;
*/
static enum parser_status parse_element(struct lexer *lexer, struct ast *node)
{
    char **data = node->data;
    if (lexer->current_tok->type == TOKEN_WORD)
    {
        struct token *start = lexer_pop(lexer);
        data = add_to_data(data, start->value);
        free(start);
        while (exp_node_continue(lexer->current_tok->type))
        {
            struct token *tmp = lexer_pop(lexer);
            data = add_to_data(data, tmp->value);
            free(tmp);
        }
        node->data = data;
        return PARSER_OK;
    }
    else
    {
        return PARSER_NOK;
    }
}

/*
    redirection = [IONUMBER] ( '>' |  '<' | '>>' | '>&' | '<&' | '>|' | '<>' )
   WORD ;
*/
static enum parser_status parse_redirection(struct lexer *lexer,
                                            struct ast **node)
{
    struct ast *new_redir = ast_genesis(AST_REDIR);
    char **data = new_redir->data;
    if (lexer->current_tok->type == TOKEN_REDIR)
    {
        struct token *start = lexer_pop(lexer);
        data = add_to_data(data, start->value);
        free(start);
        new_redir->data = data;
        *node = new_redir;
        return parse_element(lexer, new_redir);
    }
    else
    {
        *node = new_redir;
        return PARSER_NOK;
    }
}

/*
    prefix = redirection ;
*/
static enum parser_status parse_prefix(struct lexer *lexer, struct ast **node)
{
    if (parse_redirection(lexer, node) == PARSER_OK)
    {
        return PARSER_OK;
    }
    else
    {
        return PARSER_NOK;
    }
}

/*
    simple_command =
        prefix { prefix } <- doesnt exist yet do not implement
    |   { prefix } WORD { element }
    ;
*/
static enum parser_status parse_simple_command(struct lexer *lexer,
                                               struct ast **node)
{
    struct ast *new_cmd = ast_genesis(AST_CMD);

    struct ast *child_redir = NULL;
    if (lexer->current_tok->type == TOKEN_REDIR)
    {
        if (parse_prefix(lexer, &child_redir) == PARSER_NOK)
        {
            ast_destroy(child_redir);
            return PARSER_NOK;
        }
        new_cmd = add_child_to_parent(new_cmd, child_redir);
    }

    if (lexer->current_tok->type == TOKEN_WORD
        && parse_element(lexer, new_cmd) == PARSER_OK)
    {
        *node = new_cmd;

        if (lexer->current_tok->type == TOKEN_REDIR)
        {
            struct ast *new_redir = NULL;
            if (parse_redirection(lexer, &new_redir) == PARSER_NOK)
            {
                ast_destroy(new_redir);
                return PARSER_NOK;
            }
            add_child_to_parent(new_cmd, new_redir);
        }
        return PARSER_OK;
    }
    else
    {
        ast_destroy(new_cmd);
        return PARSER_NOK;
    }
}

/*
    compound_list = {'\n'} and_or { ( ; | "\n") {"\n"} and_or } [ ; ] {'\n'} ;
*/
static enum parser_status parse_compound_list(struct lexer *lexer,
                                              struct ast **node)
{
    newline_cleanser(lexer);
    struct ast *new_clist = ast_genesis(AST_LIST);
    *node = new_clist;

    struct ast *new_and_or = NULL;
    if (parse_and_or(lexer, &new_and_or) == PARSER_NOK)
    {
        ast_destroy(new_and_or);
        return PARSER_NOK;
    }
    new_clist = add_child_to_parent(new_clist, new_and_or);

    newline_cleanser(lexer);
    while (list_node_continue(lexer->current_tok->type))
    {
        struct ast *node = NULL;
        if (parse_and_or(lexer, &node) == PARSER_NOK)
        {
            ast_destroy(node);
            return PARSER_NOK;
        }
        new_clist = add_child_to_parent(new_clist, node);
        newline_cleanser(lexer);
    }

    return PARSER_OK;
}

/*
    rule_until = 'until' compound_list 'do' compound_list 'done' ;
*/
static enum parser_status parse_until(struct lexer *lexer, struct ast **node)
{
    struct ast *new_until = ast_genesis(AST_UNTIL);
    *node = new_until;
    free_token(lexer_pop(lexer));

    struct ast *condition_list = NULL;
    if (parse_compound_list(lexer, &condition_list) == PARSER_NOK)
    {
        ast_destroy(condition_list);
        return PARSER_NOK;
    }
    new_until = add_child_to_parent(new_until, condition_list);

    struct ast *do_list = NULL;
    if (!(lexer->current_tok->type == TOKEN_DO))
    {
        return PARSER_NOK;
    }
    else
    {
        free_token(lexer_pop(lexer));
        if (parse_compound_list(lexer, &do_list) == PARSER_NOK)
        {
            ast_destroy(do_list);
            return PARSER_NOK;
        }
        new_until = add_child_to_parent(new_until, do_list);
    }

    if (!(lexer->current_tok->type == TOKEN_DONE))
    {
        return PARSER_NOK;
    }
    free_token(lexer_pop(lexer));

    return PARSER_OK;
}

/*
    rule_while = 'while' compound_list 'do' compound_list 'done' ;
*/
static enum parser_status parse_while(struct lexer *lexer, struct ast **node)
{
    struct ast *new_while = ast_genesis(AST_WHILE);
    *node = new_while;
    free_token(lexer_pop(lexer));

    struct ast *condition_list = NULL;
    if (parse_compound_list(lexer, &condition_list) == PARSER_NOK)
    {
        ast_destroy(condition_list);
        return PARSER_NOK;
    }
    new_while = add_child_to_parent(new_while, condition_list);

    struct ast *do_list = NULL;
    if (!(lexer->current_tok->type == TOKEN_DO))
    {
        return PARSER_NOK;
    }
    else
    {
        free_token(lexer_pop(lexer));
        if (parse_compound_list(lexer, &do_list) == PARSER_NOK)
        {
            ast_destroy(do_list);
            return PARSER_NOK;
        }
        new_while = add_child_to_parent(new_while, do_list);
    }

    if (!(lexer->current_tok->type == TOKEN_DONE))
    {
        return PARSER_NOK;
    }
    free_token(lexer_pop(lexer));

    return PARSER_OK;
}
/*
    elif
*/
static enum parser_status parse_elif(struct lexer *lexer, struct ast **node)
{
    struct ast *new_if = ast_genesis(AST_IF);
    *node = new_if;
    free_token(lexer_pop(lexer));

    struct ast *condition_list = NULL;
    if (parse_compound_list(lexer, &condition_list) == PARSER_NOK)
    {
        ast_destroy(condition_list);
        return PARSER_NOK;
    }
    new_if = add_child_to_parent(new_if, condition_list);

    struct ast *then_list = NULL;
    if (lexer->current_tok->type != TOKEN_THEN)
    {
        return PARSER_NOK;
    }
    free_token(lexer_pop(lexer));

    if (parse_compound_list(lexer, &then_list) == PARSER_NOK)
    {
        ast_destroy(then_list);
        return PARSER_NOK;
    }
    new_if = add_child_to_parent(new_if, then_list);

    struct ast *else_clause = NULL;
    if (parse_else(lexer, &else_clause) == PARSER_NOK)
    {
        ast_destroy(else_clause);
        return PARSER_NOK;
    }
    new_if = add_child_to_parent(new_if, else_clause);

    return PARSER_OK;
}

/*
    else_clause =
        'else' compound_list
    |   'elif' = rule_if
    ;
*/
enum parser_status parse_else(struct lexer *lexer, struct ast **node)
{
    if (lexer->current_tok->type == TOKEN_ELIF)
    {
        return parse_elif(lexer, node);
    }
    else if (lexer->current_tok->type == TOKEN_ELSE)
    {
        free_token(lexer_pop(lexer));
        return parse_compound_list(lexer, node);
    }
    else
    {
        return PARSER_NOK;
    }
}

/*
    rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;
*/
enum parser_status parse_if(struct lexer *lexer, struct ast **node)
{
    struct ast *new_if = ast_genesis(AST_IF);
    *node = new_if;
    free_token(lexer_pop(lexer));

    struct ast *condition_list = NULL;
    if (parse_compound_list(lexer, &condition_list) == PARSER_NOK)
    {
        ast_destroy(condition_list);
        return PARSER_NOK;
    }
    new_if = add_child_to_parent(new_if, condition_list);

    struct ast *then_list = NULL;
    if (lexer->current_tok->type != TOKEN_THEN)
    {
        return PARSER_NOK;
    }
    free_token(lexer_pop(lexer));

    if (parse_compound_list(lexer, &then_list) == PARSER_NOK)
    {
        ast_destroy(then_list);
        return PARSER_NOK;
    }
    new_if = add_child_to_parent(new_if, then_list);

    if (lexer->current_tok->type != TOKEN_FI)
    {
        struct ast *else_clause = NULL;
        if (parse_else(lexer, &else_clause) == PARSER_NOK)
        {
            ast_destroy(else_clause);
            return PARSER_NOK;
        }
        new_if = add_child_to_parent(new_if, else_clause);
    }

    if (lexer->current_tok->type != TOKEN_FI)
    {
        return PARSER_NOK;
    }
    free_token(lexer_pop(lexer));

    return PARSER_OK;
}

/*
    shell_command =
        rule_if
    |   rule_while
    |   rule_until
    ;
*/
static enum parser_status parse_shell_command(struct lexer *lexer,
                                              struct ast **node)
{
    if (lexer->current_tok->type == TOKEN_IF)
    {
        return parse_if(lexer, node);
    }
    else if (lexer->current_tok->type == TOKEN_WHILE)
    {
        return parse_while(lexer, node);
    }
    else if (lexer->current_tok->type == TOKEN_UNTIL)
    {
        return parse_until(lexer, node);
    }
    else
    {
        return PARSER_NOK;
    }
}

/*
    command =
        simple_command
    |   shell_command {redirect}
    ;
*/
static enum parser_status parse_command(struct lexer *lexer, struct ast **node)
{
    if (is_shell_command(lexer->current_tok->type))
    {
        return parse_shell_command(lexer, node);
    }
    else if (lexer->current_tok->type == TOKEN_WORD)
    {
        return parse_simple_command(lexer, node);
    }
    else
    {
        return PARSER_NOK;
    }
}

/*
    pipeline = ['!'] command { '|' {'\n'} command } ;
*/
static enum parser_status parse_pipeline(struct lexer *lexer, struct ast **node)
{
    int is_neg = -1;
    while (lexer->current_tok->type == TOKEN_NOT)
    {
        free_token(lexer_pop(lexer));
        is_neg = -is_neg;
    }
    if (is_neg == 1)
    {
        struct ast *new_not = ast_genesis(AST_NOT);
        *node = new_not;
    }

    struct ast *new_child = NULL;
    if (parse_command(lexer, &new_child) == PARSER_NOK)
    {
        ast_destroy(new_child);
        return PARSER_NOK;
    }

    struct ast *new_pipe = ast_genesis(AST_PIPE);
    new_pipe = add_child_to_parent(new_pipe, new_child);
    if (*node == NULL)
    {
        *node = new_pipe;
    }
    else
    {
        *node = add_child_to_parent(*node, new_pipe);
    }

    while (lexer->current_tok->type == TOKEN_PIPE)
    {
        free_token(lexer_pop(lexer));
        newline_cleanser(lexer);
        new_child = NULL;
        if (parse_command(lexer, &new_child) == PARSER_NOK)
        {
            ast_destroy(new_child);
            return PARSER_NOK;
        }
        new_pipe = add_child_to_parent(new_pipe, new_child);
    }

    return PARSER_OK;
}

/*
    and_or = pipeline { ( '&&' | '||' ) {'\n'} pipeline } ;
*/
enum parser_status parse_and_or(struct lexer *lexer, struct ast **node)
{
    struct ast *node_pipe = NULL;
    if (parse_pipeline(lexer, &node_pipe) == PARSER_NOK)
    {
        ast_destroy(node_pipe);
        return PARSER_NOK;
    }
    newline_cleanser(lexer);

    while (lexer->current_tok->type == TOKEN_AND
           || lexer->current_tok->type == TOKEN_OR)
    {
        if (lexer->current_tok->type == TOKEN_AND)
        {
            struct ast *new_and = ast_genesis(AST_AND);
            *node = new_and;
            new_and = add_child_to_parent(new_and, node_pipe);
            free_token(lexer_pop(lexer));
            newline_cleanser(lexer);

            struct ast *node_child = NULL;
            if (parse_and_or(lexer, &node_child) == PARSER_NOK)
            {
                ast_destroy(node_child);
                return PARSER_NOK;
            }
            new_and = add_child_to_parent(new_and, node_child);
        }
        else if (lexer->current_tok->type == TOKEN_OR)
        {
            struct ast *new_or = ast_genesis(AST_OR);
            *node = new_or;
            new_or = add_child_to_parent(new_or, node_pipe);
            free_token(lexer_pop(lexer));
            newline_cleanser(lexer);

            struct ast *node_child = NULL;
            if (parse_and_or(lexer, &node_child) == PARSER_NOK)
            {
                ast_destroy(node_child);
                return PARSER_NOK;
            }
            new_or = add_child_to_parent(new_or, node_child);
        }
    }

    if (*node == NULL)
    {
        *node = node_pipe;
    }

    return PARSER_OK;
}

/*
    list = and_or { ( ';' ) and_or } [ ';' ] ;
*/
static enum parser_status parse_list(struct lexer *lexer, struct ast **node)
{
    struct ast *new_list = ast_genesis(AST_LIST);
    *node = new_list;
    while (list_node_continue(lexer->current_tok->type))
    {
        struct ast *ast_node = NULL;
        if (parse_and_or(lexer, &ast_node) == PARSER_NOK)
        {
            ast_destroy(ast_node);
            return PARSER_NOK;
        }
        new_list = add_child_to_parent(new_list, ast_node);

        if (lexer->current_tok->type == TOKEN_NL)
        {
            free_token(lexer_pop(lexer));
        }
    }
    return PARSER_OK;
}

/*
    input =
        list '\n'
    |   list EOF
    |   '\n'
    |   EOF
    ;
*/
enum parser_status parse(struct ast **root, struct lexer *lexer)
{
    struct ast *ast_node = NULL;

    if (lexer->current_tok->type == TOKEN_NL)
    {
        free_token(lexer_pop(lexer));
        return PARSER_OK;
    }
    else if (lexer->current_tok->type == TOKEN_EOF)
    {
        return PARSER_OK;
    }
    else if (parse_list(lexer, &ast_node) == PARSER_OK)
    {
        *root = ast_node;
        parse(root, lexer);
        return PARSER_OK;
    }
    else
    {
        ast_destroy(ast_node);
        return PARSER_NOK;
    }
}
