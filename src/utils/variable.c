#include "variable.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct var *init_var(char *data, enum var_type type, char *name)
{
    struct var *res = malloc(sizeof(struct var));
    res->name = name;
    res->type = type;
    if (type == INT)
    {
        res->data.numb = atoi(data);
        res->len = 1;
    }
    else if (type == STRING)
    {
        res->data.string = data;
        res->len = 1;
    }
    else
    {
        // later
    }
    return res;
}

void destroy_var(struct var *v)
{
    free(v->name);
    free(v);
}

struct var *set_var(struct var *var, char *value)
{
    free(var->name);
    if (var->type == INT)
    {
        var->data.numb = atoi(value);
    }
    else if (var->type == STRING)
    {
        var->data.string = value;
    }
    else
    {
        // later
    }
    return var;
}

struct var_holder *init_var_holder(void)
{
    struct var_holder *vh = malloc(sizeof(struct var_holder));
    vh->user_variables = NULL;
    vh->size = 0;
    vh->arg_variables = NULL;
    char *buffer = malloc(1024);
    buffer = getcwd(buffer, 1024);
    vh->env_variables[0] = NULL;
    vh->env_variables[1] = buffer;
    vh->env_variables[2] = malloc(4 * sizeof(char));
    vh->env_variables[2][0] = ' ';
    vh->env_variables[2][1] = '\t';
    vh->env_variables[2][2] = '\n';
    vh->env_variables[2][3] = '\0';
    return vh;
}

int unset_variable(size_t index, struct var_holder *vh)
{
    destroy_var(vh->user_variables[index]);
    for (size_t i = index; i < vh->size - 1; i++)
    {
        vh->user_variables[i] = vh->user_variables[i + 1];
    }
    vh->size--;
    vh->user_variables = realloc(vh->user_variables, vh->size);
    return 0;
}

void destroy_holder(struct var_holder *vh)
{
    while (vh->size != 0)
    {
        unset_variable(0, vh);
    }
    free(vh->user_variables);
    free(vh->arg_variables);
    for (int i = 0; i < NMB_ENV_VARS; i++)
    {
        free(vh->env_variables[i]);
    }
    free(vh);
}

size_t access_variable(char *name, struct var_holder *vh)
{
    size_t i = 0;
    while (i < vh->size)
    {
        if (strcmp(name, vh->user_variables[i]->name) == 0)
        {
            return i;
        }
        i++;
    }
    return i;
}

int set_variable(char *name, char *value, enum var_type type,
                 struct var_holder *vh)
{
    size_t index = access_variable(name, vh);
    if (index == vh->size)
    {
        vh->size++;
        vh->user_variables =
            realloc(vh->user_variables, vh->size * sizeof(struct var *));
        vh->user_variables[index] = init_var(value, type, name);
    }
    else
    {
        vh->user_variables[index] = set_var(vh->user_variables[index], value);
    }
    return 0;
}

struct var *get_variable(char *name, struct var_holder *vh)
{
    size_t index = access_variable(name, vh);
    if (index == vh->size)
    {
        return NULL;
    }
    return vh->user_variables[index];
}
