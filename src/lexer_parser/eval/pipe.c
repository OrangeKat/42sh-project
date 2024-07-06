#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "eval.h"

void pipes_destroy(int **pipes, int num_commands, char ***argvs)
{
    for (int i = 0; i < num_commands - 1; i++)
    {
        free(pipes[i]);
    }
    free(pipes);
    free(argvs);
}

void pipes_close(int **pipes, int num_commands)
{
    for (int j = 0; j < num_commands - 1; ++j)
    {
        close(pipes[j][0]);
        close(pipes[j][1]);
    }
}

int run_piped_commands(struct ast *pipe_node)
{
    size_t num_commands = pipe_node->nb_children;
    char ***argvs = malloc(sizeof(char **) * pipe_node->nb_children);
    for (size_t i = 0; i < pipe_node->nb_children; i++)
    {
        argvs[i] = pipe_node->children[i]->data;
    }

    int **pipes = calloc(num_commands, sizeof(int *));
    for (size_t i = 0; i < num_commands - 1; i++)
    {
        pipes[i] = malloc(sizeof(int) * 2);
    }

    for (size_t i = 0; i < num_commands - 1; ++i)
    {
        if (pipe(pipes[i]) == -1)
        {
            pipes_destroy(pipes, num_commands, argvs);
            return 0;
        }
    }

    for (size_t i = 0; i < num_commands; ++i)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            pipes_destroy(pipes, num_commands, argvs);
            return 0;
        }

        if (pid == 0)
        {
            if (i != 0)
            {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1)
                {
                    pipes_destroy(pipes, num_commands, argvs);
                    return 0;
                }
            }

            if (i != num_commands - 1)
            {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1)
                {
                    pipes_destroy(pipes, num_commands, argvs);
                    return 0;
                }
            }

            pipes_close(pipes, num_commands);

            size_t size = 0;
            for (size = 0; argvs[i][size] != NULL; size++)
                ;
            int ret = cmd_handler(argvs[i], size);
            pipes_destroy(pipes, num_commands, argvs);
            return ret;
        }
    }

    pipes_close(pipes, num_commands);
    for (size_t i = 0; i < num_commands; ++i)
    {
        wait(NULL);
    }

    pipes_destroy(pipes, num_commands, argvs);
    return 1;
}
