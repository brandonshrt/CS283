1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

    > **Answer**: To ensure that all of the child process finish, I used the waitpid() function to make sure that the children finish before the shell accepts input again. If I forgot the waitpid() function it would create a bunch of zombie processes for the children, which would create system resource exhaustion, and make the command prompts begin to overlap. 

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

    > **Answer**: It's necessary to close the unused pipes because after dup2() redirects the pipe, the original file descriptors stay open unless they are explicitly closed. If the pipes are left open then it could create hanging processes, meaning that in the case of 'ls | grep', the pipe would have an open writer so grep continues to wait for more input.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

    > **Answer**: The cd command can be executed during the shell's own process by using chdir(), which will update the shell's cwd. If cd were to be implemented as an external process it would need to call chdir() in the child process which would only change the cwd in the child process and not in the shell's process, so the command would essentially do nothing. This would also cause anything piped into cd to fail because it is not working in the shell process. 

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

    > **Answer**: In command_list_t I would change commands to be a pointer to a dynamically allocated array, which would allow me to add as many pipes as I need. In my build_cmd_list() function, I would allocate a small number of pipes to begin with like 2 or 4 and whenever more are need I would double it when needed. 