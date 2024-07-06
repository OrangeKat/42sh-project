#ifndef BUILTIN_H
#define BUILTIN_H

#include <stddef.h>

int bin_true(void);
int bin_false(void);
int echo(char *args[], size_t argnum);
int bin_exit(char **args, size_t argnum);
int cd(char **args, size_t argsnum);

#endif // BUILTIN_H
