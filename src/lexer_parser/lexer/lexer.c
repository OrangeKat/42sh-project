#include "lexer.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/token_utils.h"
#include "../../utils/variable.h"
#include "token.h"

struct lexer *lexer_genesis(FILE *input_file)
{
    struct lexer *new = malloc(sizeof(struct lexer));
    new->input_file = input_file;
    new->separator = 0;
    new->current_tok = parse_input_for_tok(new);
    new->offset = ftell(input_file);
    if (new->current_tok->type == TOKEN_VAR)
    {
        extern struct var_holder *g_vh;
        char *var_name = new->current_tok->value;
        char *var_val = var_name;
        for (size_t i = 0; var_name[i] != '='; i++)
        {
            var_val++;
        }
        *var_val = '\0';
        var_val++;
        set_variable(var_name, var_val, STRING, g_vh);
        free(new->current_tok);

        new->current_tok = parse_input_for_tok(new);
        new->offset = ftell(input_file);
    }

    if (new->separator)
    {
        new->offset--;
        new->separator = 0;
    }
    return new;
}

void lexer_destroy(struct lexer *lexer)
{
    if (lexer->current_tok)
    {
        if (lexer->current_tok->value)
        {
            free(lexer->current_tok->value);
        }
        free(lexer->current_tok);
    }
    free(lexer);
}

struct token *lexer_single_quote(struct lexer *lexer, char *str,
                                 struct token *res)
{
    free(str);
    res->value = get_string(lexer->input_file);
    if (res->value == NULL)
    {
        res->type = TOKEN_ERROR;
        return res;
    }

    res->type = TOKEN_WORD;
    return res;
}

struct token *lexer_double_quote(struct lexer *lexer, char *str,
                                 struct token *res)
{
    free(str);
    res->type = TOKEN_WORD;
    res->value = get_double_quote(lexer->input_file);
    if (res->value == NULL)
    {
        res->type = TOKEN_ERROR;
    }
    return res;
}

char *escape_char(struct lexer *lexer, char *str, size_t *size)
{
    char c;
    c = fgetc(lexer->input_file);
    if (c == EOF)
    {
        free(str);
        return NULL;
    }
    else
    {
        str[*size - 1] = c;
        (*size)++;
        str = realloc(str, *size);
        return str;
    }
}

struct token *new_line(struct lexer *lexer, char *str, struct token *res,
                       size_t size)
{
    if (size == 1)
    {
        free(str);
        res->type = TOKEN_NL;
        return res;
    }
    str[size - 1] = '\0';
    res = set_token(res, str);
    lexer->separator = 1;
    return res;
}

struct token *parse_input_for_tok(struct lexer *lexer)
{
    struct token *res = malloc(sizeof(struct token));
    res->value = NULL;
    size_t size = 1;
    char *str = calloc(sizeof(char), size);
    char c;
    while ((c = fgetc(lexer->input_file)) != EOF && is_separator(c))
    {
        continue;
    }
    if (c != EOF)
    {
        if (c == '#')
        {
            skip_comment(lexer);
        }
        else
        {
            fseek(lexer->input_file, -1, SEEK_CUR);
        }
    }
    while ((c = fgetc(lexer->input_file)) != EOF && !is_separator(c))
    {
        if (c == '\'')
        {
            if (str[0] != '\0')
            {
                fseek(lexer->input_file, -1, SEEK_CUR);
                str[size - 1] = '\0';
                return set_token(res, str);
            }
            return lexer_single_quote(lexer, str, res);
        }
        if (c == '\"')
        {
            if (str[0] != '\0')
            {
                fseek(lexer->input_file, -1, SEEK_CUR);
                str[size - 1] = '\0';
                return set_token(res, str);
            }
            return lexer_double_quote(lexer, str, res);
        }
        if (c == '\\')
        {
            str = escape_char(lexer, str, &size);
            if (str == NULL)
            {
                res->type = TOKEN_ERROR;
                return res;
            }
            continue;
        }
        if (c == '\n' || c == ';')
        {
            /*if (size == 1)
            {
                free(str);
                res->type = TOKEN_NL;
                return res;
            }
            str[size - 1] = '\0';
            res = set_token(res, str);
            lexer->separator = 1;
            return res;*/
            return new_line(lexer, str, res, size);
        }
        str[size - 1] = c;
        size++;
        str = realloc(str, size);
    }
    str[size - 1] = '\0';
    return set_token(res, str);
}

struct token *lexer_peek(struct lexer *lexer)
{
    fseek(lexer->input_file, lexer->offset, SEEK_SET);
    struct token *tok = parse_input_for_tok(lexer);
    return tok;
}

struct token *lexer_pop(struct lexer *lexer)
{
    struct token *to_pop = lexer->current_tok;
    struct token *peek = lexer_peek(lexer);
    lexer->current_tok = peek;
    lexer->offset = ftell(lexer->input_file);
    if (lexer->separator)
    {
        lexer->offset--;
        lexer->separator = 0;
    }

    if (peek->type == TOKEN_VAR)
    {
        extern struct var_holder *g_vh;
        char *var_name = lexer->current_tok->value;
        char *var_val = var_name;
        for (size_t i = 0; var_name[i] != '='; i++)
        {
            var_val++;
        }
        *var_val = '\0';
        var_val++;
        set_variable(var_name, var_val, STRING, g_vh);
        free(peek);

        peek = lexer_peek(lexer);
        lexer->current_tok = peek;
        lexer->offset = ftell(lexer->input_file);
        if (lexer->separator)
        {
            lexer->offset--;
            lexer->separator = 0;
        }
    }
    return to_pop;
}
