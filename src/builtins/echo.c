#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"

/**
** \brief Extends two characters to their special character
** \details Extends the two characters at index and index + 1 to their special
** character, defined by new
** \param str The string to extend
** \param index The index where the two characters are located
** \param new The special character that will take place of the old two
*/
void extend_char(char *str, size_t index, char new)
{
    str[index++] = new;
    while (str[index] != '\0')
    {
        str[index] = str[index + 1];
        index++;
    }
}

/**
** \brief Extends two characters to their special character in the string
** \details Loops through the strings and extends characters when it finds them
** \param str The string to extend
** \param extend Controls if \t (two separate characters)are extended to tabs
*/
void extend_string(char *str)
{
    size_t i = 0;
    while (str[i] != '\0')
    {
        if (str[i++] == '\\')
        {
            if (str[i] == 'n')
            {
                extend_char(str, i - 1, '\n');
            }
            else if (str[i] == 't')
            {
                extend_char(str, i - 1, '\t');
            }
            else if (str[i] == '\\')
            {
                extend_char(str, i - 1, '\\');
            }
        }
    }
}

/**
** \brief Prints a string to standard output
** \details Takes a string from statard input and prints it to standard output
** \param str The string to print
** \param extend Controls if \t (two separate characters)are extended to tabs
** and \\n to newline\n
** 0 by default or if -E, 1 if -e is specified
** \return Returns 0 in case of success, 1 in case of failure
*/
int echosingle(char *str, int extend)
{
    if (extend)
    {
        extend_string(str);
    }
    printf("%s", str);
    return 0;
}

int argset(char *arg, int *newline, int *extend)
{
    size_t j = 1;
    int temp_n = *newline;
    int temp_e = *extend;
    while (arg[j] != '\0')
    {
        if (arg[j] == 'n')
        {
            *newline = 0;
        }
        else if (arg[j] == 'e')
        {
            *extend = 1;
        }
        else if (arg[j] == 'E')
        {
            *extend = 0;
        }
        else
        {
            *newline = temp_n;
            *extend = temp_e;
            return 0;
        }
        j++;
    }
    return 1;
}
/**
** \brief Prints the arguments separated by space
** \details Prints all of its arguments separated by a space
** \param args The list of arguments to print
** \param argnum The number of arguments
** \param extend Controls if \t (two separate characters)are extended to tabs
** and \\n to newline\n
** 0 by default or if -E, 1 if -e is specified
** \param newline controls if a newline is added at the end of the string\n
** 1 by default, 0 if -n is specified
** \return Returns 0 in case of success, 1 in case of failure
*/
int echo(char *args[], size_t argnum)
{
    int newline = 1;
    int extend = 0;
    size_t i = 1;
    while (i < argnum)
    {
        if (args[i][0] == '-')
        {
            if (!argset(args[i], &newline, &extend))
            {
                break;
            }
        }
        else
        {
            break;
        }
        i++;
    }
    while (i < argnum)
    {
        if (echosingle(args[i], extend))
            return 1;
        if (i != argnum - 1)
        {
            putchar(' ');
        }
        i++;
    }
    if (newline)
    {
        putchar('\n');
    }
    return 0;
}
