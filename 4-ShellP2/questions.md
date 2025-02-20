1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**: When we use fork() it allows us to keep the shell alive while we handle the commands. It also lets us handle an external command without disrupting the parent as well as being able to wait to for the command to finish before prompting again. 

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**: In my implementation, it doesn't create a child then prints an error to stderr and returns ERR_MEMORY.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**: It takes two arguments, file (the command) and argv (the arguments). If the file has a '/' in it, it'll be treated as a path and if not, it searches for an executable in the PATH environment variable to execute the command. 

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**: wait() makes sure that the parent process pauses it's execution until any child processes are finsihed. Without it the shell would prompt too soon, the outputs would overlap, zombies would accumulate and create system resource exhaustion. 

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**: WEXITSTATUS() returns the status code of the child process. This returns the exit status of the child process which can be used in debugging and indicates success/failures.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**: I created a boolean, inQuotes, which would indicate if the current char is inside of quotes or not. If it was inside of quotes then everything inside of that quote will be copied over, otherwise the code will run as it did in the last assignment by removing leading and trailing spaces from the args. 

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**: I only had to add the parsing logic for the quotes. The only challenge I had was how and where to implement it, because it would parse the quotes as args by themselves and the material inside the quote as an arg. Then it would get rid of the leading and trailing spaces and keep the quoted object as an arg. 

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**: Signals are used to notify processes of asynchronous events or to ask for specific actions to be completed. It's different than IPCs because IPCs operate on data transfers, like a pipe, whereas the signal goes to the target process immediately by the kernal. 

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**: **SIGKILL**: This is used to kill a signal, used usually when a process is unresponsive to SIGTERM or when you need to stop a process immediately. **SIGINT**: This used to stop a process from the keyboard, allows a user to press ^C to stop a command or process. **SIGALRM**: This signal is generated when the timer set by an alarm function goes off. 

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  SIGSTOP stops a process instantly and it cannot be caught or ignored like SIGINT. This ensures system-level authority, this supports job control and debugging, as well as adhering to POSIX.