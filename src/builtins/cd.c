#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../utils/variable.h"
#include "builtin.h"

int cd(char **args, size_t argsnum)
{
    extern struct var_holder *g_vh;
    if (argsnum != 2)
    {
        write(STDERR_FILENO, "cd: invalid number of arguments", 32);
        return 1;
    }
    if (strcmp("-", args[1]) == 0)
    {
        char *temp = g_vh->env_variables[SPWD];
        if (chdir(temp))
        {
            return 1;
        }
        g_vh->env_variables[SPWD] = g_vh->env_variables[SOLDPWD];
        g_vh->env_variables[SOLDPWD] = temp;
    }
    else
    {
        char *temp = g_vh->env_variables[SOLDPWD];
        if (chdir(args[1]))
        {
            return 1;
        }
        g_vh->env_variables[SOLDPWD] = g_vh->env_variables[SPWD];
        free(temp);
        char *buffer = NULL;
        buffer = getcwd(buffer, 1024);
        g_vh->env_variables[SPWD] = buffer;
    }
    return 0;
}
