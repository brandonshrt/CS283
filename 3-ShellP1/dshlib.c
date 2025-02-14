#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    int cmdCount = 0;

    while (cmd_line != NULL)
    {
        if (cmdCount > CMD_MAX)
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        // Remove leading spaces
        while(isspace(*cmd_line))
        {
            cmd_line++;
        }

        // Check if command line is empty
        if (*cmd_line == '\0')
        {
            break;
        }

        char *pipePos = strchr(cmd_line, PIPE_CHAR);  // Find the next pipe
        if (pipePos != NULL) 
        {
            *pipePos = '\0';  // Null-terminate the current command
        }

        // Check if the command is too large
        if (strlen(cmd_line) > SH_CMD_MAX) 
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        // Parse the executable 
        char *exe_start = cmd_line;
        while (isspace((unsigned char)*exe_start)) 
        {
            exe_start++;
        }

        char *exe_end = strchr(exe_start, ' ');  // Find the first space after the executable
        if (exe_end != NULL) 
        {
            *exe_end = '\0';  // Null-terminate the executable name
        }

        // Check if the executable name is too long
        if (strlen(exe_start) >= EXE_MAX) 
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        // Copy executable name
        strncpy(clist->commands[cmdCount].exe, exe_start, EXE_MAX - 1);
        clist->commands[cmdCount].exe[EXE_MAX - 1] = '\0';

        // Parse arguments if there are
        clist->commands[cmdCount].args[0] = '\0';
        char *argStart = exe_end != NULL ? exe_end + 1 : NULL;

        if (argStart != NULL) 
        {
            while (*argStart != '\0') 
            {
                // Skip leading spaces
                while (isspace(*argStart)) 
                {
                    argStart++;
                }

                // Find the next space or the end of string
                char *argEnd = strchr(argStart, ' ');
                if (argEnd != NULL) 
                {
                    *argEnd = '\0';  // Null-terminate the current argument
                }

                if (strlen(clist->commands[cmdCount].args) + strlen(argStart) + 1 >= ARG_MAX)
                {
                    return ERR_CMD_OR_ARGS_TOO_BIG;  // Avoid buffer overflow
                }

                // Concatenate the argument to the args field
                strncat(clist->commands[cmdCount].args, argStart, ARG_MAX - strlen(clist->commands[cmdCount].args) - 1);
                strncat(clist->commands[cmdCount].args, " ", ARG_MAX - strlen(clist->commands[cmdCount].args) - 1);

                // Move to the next argument
                if (argEnd != NULL) 
                {
                    argStart = argEnd + 1;
                }
                else 
                {
                    break;  // No more arguments, exit loop
                }
            }

        }


        cmdCount++;
        clist->num = cmdCount;
        //printf("[%d]\n", cmdCount);

        // Check if we encountered too many commands
        if (cmdCount > CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        if (pipePos != NULL) {
            cmd_line = pipePos + 1;
        }
        else {
            cmd_line = NULL;
        }
    }
    return OK;
}