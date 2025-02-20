#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
//void printBuff(cmd_buff_t *cmd_buff);

int exec_local_cmd_loop()
{
    char *cmd_buff;
    int rc = 0;
    cmd_buff_t cmd;

    if (alloc_cmd_buff(&cmd) != OK)
    {
        return ERR_MEMORY;   
    }
    
    // Allocate memory for the command buffer (cmd_buff)
    cmd_buff = malloc(SH_CMD_MAX * sizeof(char));
    
    if (cmd_buff == NULL) 
    {
        return ERR_MEMORY;  // Memory allocation failed
    }

    while(1){
        printf("%s", SH_PROMPT);

        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
            printf("\n");
            break;
        }

        //remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

        if (cmd_buff[0] == '\0')
        {
            continue;
        }

        // Build the cmd_buff
        rc = build_cmd_buff(cmd_buff, &cmd);
    
        switch (rc)
        {
            case ERR_MEMORY:
                return rc;
                break;
            case WARN_NO_CMDS:
                printf(CMD_WARN_NO_CMD);
                continue;
            case ERR_TOO_MANY_COMMANDS:
                printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
                continue;
            case ERR_CMD_OR_ARGS_TOO_BIG:
                printf(CMD_ERR_PIPE_LIMIT, CMD_ARGV_MAX);
                continue;
            case OK:
                break;
            default:
                break;
        }

        // Check if it's a built in command or not
        Built_In_Cmds cmd_type = match_command(cmd.argv[0]);
        if (cmd_type != BI_NOT_BI)
        {
            rc = exec_built_in_cmd(&cmd);
            // Handle built-in command
            if (rc == BI_CMD_EXIT) 
            {
                //printBuff(&cmd);
                break;
            } 
            else if (rc == BI_CMD_CD) 
            {
                // cd command has already been handled in exec_built_in_cmd, continue to next input
                continue;
            }
        } else 
        {
            rc = exec_cmd(&cmd);
            if (rc == WARN_NO_CMDS)
            {
                printf(CMD_WARN_NO_CMD);
            }

            if (rc != OK)
            {
                printf("%s: Error executing command\n", cmd.argv[0]); 
            }
        }

        //IMPLEMENT THE REST OF THE REQUIREMENTS
    }

    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

    free_cmd_buff(&cmd);
    free(cmd_buff);
    return rc;
}

int exec_cmd(cmd_buff_t *cmd)
{
    // If no arguments are passed, return an error
    if (cmd->argc == 0) 
    {
        return WARN_NO_CMDS;
    }

    // Fork a new process to execute external commands
    pid_t pid = fork();
    if (pid == -1) 
    {
        perror("fork");
        return ERR_MEMORY;  // Error during fork
    } else if (pid == 0) 
    {
        // Execute external cmd in child process
        if (execvp(cmd->argv[0], cmd->argv) == -1) 
        {
            perror("Error: Execution failure");  // Error executing the command
            exit(EXIT_FAILURE); 
        }
    } else 
    {
        // Wait for child process to finish
        int status;
        wait(&status);  // Wait for child process to complete
    }
   
    return OK;
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff)
{
    // Allocate memory for the whole command buffer
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX * sizeof(char));
    if (cmd_buff->_cmd_buffer == NULL)
    {
        return ERR_MEMORY;
    }
    
    // Set all args to NULL
    for (int i = 0; i < CMD_ARGV_MAX; i++) 
    {
        cmd_buff->argv[i] = NULL; 
    }

    // Initialize arg count to 0
    cmd_buff->argc = 0;

    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff)
{
    if (cmd_buff->_cmd_buffer != NULL)
    {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
        
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff)
{
    // Clear arg count
    cmd_buff->argc = 0;

    // Reset each arg to NULL
    for (int i = 0; i < CMD_ARGV_MAX; i++)
    {
        cmd_buff->argv[i] = NULL;
    }

    // Clear the command buffer
    if (cmd_buff->_cmd_buffer != NULL) 
    {
        memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX * sizeof(char));
    }

    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    clear_cmd_buff(cmd_buff);

    while (cmd_line != NULL && *cmd_line != '\0')
    {
        // Skip leading whitespace
        while (isspace((unsigned char)*cmd_line))
        {
            cmd_line++;
        }

        if (*cmd_line == '\0')
        {
            break;  // No more input
        }

        if (cmd_buff->argc >= CMD_ARGV_MAX)
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        char *arg_start;
        char *arg_end;

        // Handle quoted string
        if (*cmd_line == '"')
        {
            cmd_line++;  // Skip opening quote
            arg_start = cmd_line;

            // Find closing quote
            arg_end = strchr(cmd_line, '"');
            if (arg_end == NULL)
            {
                return ERR_CMD_OR_ARGS_TOO_BIG;  // Unterminated quote
            }

            *arg_end = '\0';  // Null-terminate at closing quote
            cmd_line = arg_end + 1;  // Move past closing quote
        }
        else  // Handle unquoted arg
        {
            arg_start = cmd_line;

            // Find end of unquoted arg 
            while (*cmd_line != '\0' && !isspace((unsigned char)*cmd_line) && *cmd_line != '"')
            {
                cmd_line++;
            }

            arg_end = cmd_line;
            if (*cmd_line != '\0')
            {
                *arg_end = '\0';  // Null-terminate at space
                cmd_line++;  // Move past the null terminator
            }
        }

        // Add the arg to argv
        if (strlen(arg_start) >= EXE_MAX)
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        cmd_buff->argv[cmd_buff->argc] = arg_start;
        cmd_buff->argc++;

        // Check for pipe (if supporting piping)
        char *pipePos = strchr(arg_start, PIPE_CHAR);
        if (pipePos != NULL)
        {
            *pipePos = '\0';
            cmd_line = pipePos + 1;
            break;  // Handle piping separately if needed
        }
    }

    if (cmd_buff->argc == 0)
    {
        return WARN_NO_CMDS;
    }

    // Debug
    // printf("Final: argc=%d\n", cmd_buff->argc);
    // for (int i = 0; i < cmd_buff->argc; i++)
    // {
    //     printf("argv[%d]=%s\n", i, cmd_buff->argv[i]);
    // }

    return OK;
}

Built_In_Cmds match_command(const char *input)
{
    if (strcmp(input, EXIT_CMD) == 0) 
    {
        return BI_CMD_EXIT;
    } else if (strcmp(input, "cd") == 0) 
    {
        return BI_CMD_CD;
    } else 
    {
        return BI_NOT_BI;
    }
} 

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd)
{
    Built_In_Cmds cmd_type = match_command(cmd->argv[0]);
    if (cmd_type == BI_CMD_EXIT) 
    {
        return BI_CMD_EXIT;  // Exit the shell
    } else if (cmd_type == BI_CMD_CD) 
    {
        if (cmd->argc > 1) 
        {  
            if (chdir(cmd->argv[1]) != 0) 
            {
                perror("cd");
            }
        } 
        return BI_CMD_CD;
    } 

    return BI_NOT_BI;
}