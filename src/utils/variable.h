#ifndef VARIABLE_H
#define VARIABLE_H

#include <stddef.h>

#define SA 0
#define SSTAR 1
#define SQM 2
#define SS 3
#define NMB_SPECIAL_VARS 4
#define SOLDPWD 0
#define SPWD 1
#define SIFS 2
#define NMB_ENV_VARS 3

enum var_type
{
    INT = 0,
    STRING,
    LIST,
    DELIM,
};

union var_value
{
    int numb;
    char *string;
    char **list;
};

struct var
{
    enum var_type type;
    union var_value data;
    char *name;
    size_t len;
};

struct var_holder
{
    struct var **user_variables;
    size_t size;
    char *spec_variables[4];
    struct var *arg_variables;
    char *env_variables[4];
};

struct var *init_var(char *data, enum var_type type, char *name);

struct var_holder *init_var_holder(void);

void destroy_holder(struct var_holder *vh);

int set_variable(char *name, char *value, enum var_type type,
                 struct var_holder *vh);

size_t access_variable(char *name, struct var_holder *vh);

struct var *get_variable(char *name, struct var_holder *vh);

#endif /* VARIABLE_H */
