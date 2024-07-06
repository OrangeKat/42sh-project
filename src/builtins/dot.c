#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lexer_parser/ast/ast.h"
#include "../lexer_parser/eval/eval.h"
#include "../lexer_parser/lexer/lexer.h"
#include "../lexer_parser/parser/parser.h"
#include "../utils/file_to_string.h"
#include "../utils/variable.h"
#include "builtin.h"

static int file_handler(FILE **f, char **argv)
{
    *f = fopen(argv[1], "r");
    // error
    if (*f == NULL)
    {
        err(-1, "not a file");
    }
    return 0;
}

int dot(char **data)
{
    FILE *f = NULL;
    int file_val = file_handler(&f, data);
    if (file_val == -1)
    {
        return -1;
    }

    struct lexer *lexer = lexer_genesis(f);
    struct ast *tree_root = NULL;
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
        err(2, "Wrong grammar");
    }

    int ret_val = ast_eval(tree_root);
    if (!ret_val)
    {
        lexer_destroy(lexer);
        return 1;
    }

    if (tree_root)
    {
        ast_destroy(tree_root);
    }
    lexer_destroy(lexer);
    fclose(f);
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
