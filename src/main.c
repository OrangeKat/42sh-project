#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins/builtin.h"
#include "lexer_parser/ast/ast.h"
#include "lexer_parser/eval/eval.h"
#include "lexer_parser/lexer/lexer.h"
#include "lexer_parser/parser/parser.h"
#include "utils/variable.h"

struct var_holder *g_vh = NULL;

char *stdin_handler(FILE **f, char *buffer, size_t capacity)
{
    size_t length = 0;
    char tmp_buff[1];
    while (read(STDIN_FILENO, tmp_buff, 1) > 0)
    {
        if (length + 1 == capacity)
        {
            capacity *= 2;
            buffer = realloc(buffer, capacity);
            if (!buffer)
            {
                free(buffer);
                return NULL;
            }
        }
        buffer[length++] = tmp_buff[0];
    }
    buffer[++length] = '\0';
    *f = fmemopen(buffer, length - 1, "r");
    return buffer;
}

int command_line_handler(FILE **f, char **argv, int argc, char **buffer)
{
    size_t length = 1;
    for (int i = 2; i < argc; i++)
    {
        length += strlen(argv[i]) + 1;
    }
    *buffer = malloc(length);
    *buffer[0] = '\0';
    for (int i = 2; i < argc; i++)
    {
        strcat(*buffer, argv[i]);
        if (i < argc - 1)
        {
            strcat(*buffer, " ");
        }
    }
    *f = fmemopen(*buffer, strlen(*buffer), "r");
    return 0;
}

int file_handler(FILE **f, char **argv)
{
    *f = fopen(argv[1], "r");
    // error
    if (*f == NULL)
    {
        err(-1, "not a file");
    }
    return 0;
}

static int file_allocator(FILE **f, int argc, char **argv, char **buffer)
{
    // check if it there is a string argument
    if (strcmp("-c", argv[1]) == 0)
    {
        int i;
        if (argc <= 2)
        {
            err(2, "missing an argument");
        }
        int is_empty = 1;
        for (i = 2; i < argc; i++)
        {
            if (strlen(argv[i]) != 0)
            {
                is_empty = 0;
            }
        }
        if (is_empty)
        {
            return 3;
        }
        command_line_handler(f, argv, argc, buffer);
    }
    // read from a file
    else
    {
        file_handler(f, argv);
    }
    if (f == NULL)
    {
        return 0;
    }
    return 1;
}

int loop_parse(struct ast *tree_root, struct lexer *lexer, char *buffer,
               int ret_val)
{
    if (parse(&tree_root, lexer) != PARSER_OK)
    {
        if (lexer)
        {
            lexer_destroy(lexer);
        }
        if (tree_root)
        {
            ast_destroy(tree_root);
        }
        free(buffer);
        err(2, "Wrong grammar");
    }

    ret_val = ast_eval(tree_root);
    if (!ret_val)
    {
        ast_destroy(tree_root);
        return 0;
    }
    if (tree_root)
    {
        ast_destroy(tree_root);
    }
    tree_root = NULL;
    return ret_val;
}

int main(int argc, char **argv)
{
    FILE *f = NULL;
    size_t capacity = 64;
    char *buffer = malloc(capacity);
    char *tmp = NULL;
    if (argc == 1)
    {
        if (strlen(buffer = stdin_handler(&f, buffer, capacity)) == 1)
        {
            err(2, "expected a non empty input");
        }
    }
    else
    {
        int file_val = file_allocator(&f, argc, argv, &tmp);
        if (file_val == 3)
        {
            free(buffer);
            return 0;
        }
    }
    g_vh = init_var_holder();

    struct lexer *lexer = lexer_genesis(f);
    struct ast *tree_root = NULL;
    int ret_val = 1;
    while (ret_val == 1 && lexer->current_tok->type != TOKEN_EOF)
    {
        ret_val = loop_parse(tree_root, lexer, buffer, ret_val);
    }

    lexer_destroy(lexer);
    fclose(f);
    if (tmp)
    {
        free(tmp);
    }
    free(buffer);
    ast_destroy(tree_root);
    destroy_holder(g_vh);
    if (!ret_val)
    {
        return 1;
    }
    if (ret_val < -1)
    {
        return ret_val + 257;
    }
    if (ret_val != 127)
    {
        return 0;
    }
    err(127, "something went wrong when eval do did done his thing :C");
}
