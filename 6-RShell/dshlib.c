#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

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
int exec_local_cmd_loop()
{
    // Allocate space for the command buff
    char *cmd_buff = malloc(SH_CMD_MAX * sizeof(char));
    if (!cmd_buff) 
    {
        return ERR_MEMORY;
    }
    
    // Create an empty command list
    command_list_t clist = {0};
    int rc = OK;

    while (1) {
        printf("%s", SH_PROMPT);
        
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) { 
            printf("\n");
            free(cmd_buff);
            return OK;
        }
        
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
        
        // Check if there is a command
        if (strlen(cmd_buff) == 0) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        // Check if the command is 'exit'
        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            free(cmd_buff);
            return OK;
        }

        rc = build_cmd_list(cmd_buff, &clist);

        // Print errors if found
        if (rc != OK) {
            switch (rc) {
                case WARN_NO_CMDS:
                    printf(CMD_WARN_NO_CMD);
                    continue;
                case ERR_TOO_MANY_COMMANDS:
                    printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
                    continue;
                case ERR_CMD_OR_ARGS_TOO_BIG:
                    printf("error: too many arguments, limit is %d\n", CMD_ARGV_MAX);
                    continue;
                case ERR_EXEC_CMD:
                    printf("Error: Execution failure");
                    continue;
                case ERR_MEMORY:
                    free(cmd_buff);
                    return ERR_MEMORY;
                default:
                    continue;
            }
        }

        // Check for built-in commands (only if single command)
        if (clist.num == 1) {
            Built_In_Cmds cmd_type = match_command(clist.commands[0].argv[0]);
            if (cmd_type != BI_NOT_BI) {
                rc = exec_built_in_cmd(&clist.commands[0]);
                free_cmd_list(&clist);
                if (cmd_type == BI_CMD_EXIT) {
                    free(cmd_buff);
                    return OK;
                }
                continue;
            }
        }

        // Execute pipeline for external commands
        rc = execute_pipeline(&clist);
        if (rc != OK) {
            printf("Error: Execution failure\n");
        }
        
        free_cmd_list(&clist);
    }

    free(cmd_buff);  // Never reached due to while(1), but good practice
    return OK;
}

int exec_cmd(cmd_buff_t *cmd)
{
    // Check if anything is passed
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
    // Free each buffer in cmd_buff
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
    
    // Use _cmd_buffer to store the string content
    char *buffer_pos = cmd_buff->_cmd_buffer;
    int buffer_remaining = SH_CMD_MAX;

    // Loop through the whole line
    while (cmd_line != NULL && *cmd_line != '\0')
    {
        // Skip leading white space
        while (isspace((char)*cmd_line))
        {
            cmd_line++;
        }
            
        // Check if we hit the end of the line
        if (*cmd_line == '\0')
        {
            break;
        }
            
        // Check if there are too many args
        if (cmd_buff->argc > CMD_ARGV_MAX)
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
            
        char *arg_start;
        int arg_len;

        // Handle quoted arguments
        if (*cmd_line == '"')
        {
            cmd_line++;
            arg_start = cmd_line;
            char *arg_end = strchr(cmd_line, '"');
            if (!arg_end)
                return ERR_CMD_OR_ARGS_TOO_BIG;
            arg_len = arg_end - arg_start;
            cmd_line = arg_end + 1;
        }
        else
        {
            arg_start = cmd_line;
            while (*cmd_line && !isspace((unsigned char)*cmd_line) && *cmd_line != '"')
                cmd_line++;
            arg_len = cmd_line - arg_start;
            if (*cmd_line)
                cmd_line++;
        }

        // Check if the input is too long
        if (arg_len >= buffer_remaining)
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
            
        // Copy argument to buffer
        strncpy(buffer_pos, arg_start, arg_len);
        buffer_pos[arg_len] = '\0';
        cmd_buff->argv[cmd_buff->argc] = buffer_pos;
        cmd_buff->argc++;

        buffer_pos += arg_len + 1;
        buffer_remaining -= arg_len + 1;
    }

    // Check if there are any args
    if (cmd_buff->argc == 0)
    {
        return WARN_NO_CMDS;
    }
    
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
        return BI_CMD_EXIT;
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

int execute_pipeline(command_list_t *clist)
{
    int num_commands = clist->num;

    // Check if there are any commands
    if (num_commands == 0)
    {
        return WARN_NO_CMDS;
    } 
    
    int pipes[num_commands - 1][2];
    pid_t pids[num_commands];

    // Create all the pipes
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return ERR_EXEC_CMD;
        }
    }

    // Fork and execute each pipe in the pipeline
    for (int i = 0; i < num_commands; i++) {
        pids[i] = fork();
        
        if (pids[i] == -1) {
            perror("fork");
            return ERR_EXEC_CMD;
        }
        
        if (pids[i] == 0) {
            // Redirect input
            if (i > 0) {
                if (dup2(pipes[i-1][0], STDIN_FILENO) == -1) {
                    perror("dup2 input");
                    exit(EXIT_FAILURE);
                }
            }

            // Redirect output
            if (i < num_commands - 1) {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2 output");
                    exit(EXIT_FAILURE);
                }
            }

            // Close all pipe fds
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Debug: print command being executed
            // printf("Executing: ");
            // for (int k = 0; k < clist->commands[i].argc; k++) {
            //     printf("%s ", clist->commands[i].argv[k]);
            // }
            // printf("\n");

            // Run the command
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    // Close all pipe file descriptors in the parent
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for children
    int status;
    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], &status, 0);
    }

    return OK;
}

int close_cmd_buff(cmd_buff_t *cmd_buff)
{
    // Check if the command buff frees correctly
    if (free_cmd_buff(cmd_buff) != OK) 
    {
        return ERR_MEMORY;
    }
    
    // Reset all the args
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) 
    {
        cmd_buff->argv[i] = NULL;
    }
    
    return OK;
}

int free_cmd_list(command_list_t *cmd_lst)
{
    // Free each command buff in the list
    for (int i = 0; i < cmd_lst->num; i++) 
    {
        free_cmd_buff(&cmd_lst->commands[i]);
    }
    cmd_lst->num = 0;
    return OK;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) 
{
    char *cmd;
    clist->num = 0;

    // Count all commands
    char *temp = strdup(cmd_line);
    if (!temp) 
    {
        return ERR_MEMORY;
    }
    
    // Tokenize temp with the pipe as a delimiter
    cmd = strtok(temp, PIPE_STRING);
    while (cmd) 
    {
        clist->num++;
        cmd = strtok(NULL, PIPE_STRING);
    }

    // Check if number of commands exceeds limit
    if (clist->num > CMD_MAX) 
    {
        free(temp);
        return ERR_TOO_MANY_COMMANDS;
    }

    // Check if there are any commands
    if (clist->num == 0) 
    {
        free(temp);
        return WARN_NO_CMDS;
    }

    // Build commands 
    temp = strdup(cmd_line);  
    if (!temp) 
    {
        return ERR_MEMORY;
    }
    
    // Process each command in the pipeline
    cmd = strtok(temp, PIPE_STRING);
    for (int i = 0; i < clist->num; i++) {
        if (alloc_cmd_buff(&clist->commands[i]) != OK) {
            free(temp);
            return ERR_MEMORY;
        }
        
        while (isspace((unsigned char)*cmd)) cmd++;
        char *end = cmd + strlen(cmd) - 1;
        while (end > cmd && isspace((unsigned char)*end)) *end-- = '\0';

        if (strlen(cmd) == 0) 
        {
            free_cmd_list(clist);
            free(temp);
            return WARN_NO_CMDS;  // Empty pipe segment
        }

        int rc = build_cmd_buff(cmd, &clist->commands[i]);
        if (rc != OK) {
            free(temp);
            return rc;
        }
        cmd = strtok(NULL, PIPE_STRING);
    }
    
    free(temp);
    return OK;
}