1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > fgets() allows me to specify the maximum amount of characters to read from input. It also supports EOF and flexible input, unlike gets() which needs a specified input. Having flexible input means that I can input 8 commands or 1 command and have it still work properly

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > malloc allows for flexible and reliable memory. It can allocate memory at runtime and you can adjust how much memory you need instead of creating a fixed-size array.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > Having leading or trailing spaces can potentially cause memory issues when creating a command. It can also causing unforeseen issues with command execution because it would be interpreted as wrong due to the extra spaces.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > Redirecting stdout to a file with something like (ls > output.txt). Redirecting stderr to an output file. Also redirecting both stdout and stderr to a file. 


- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > Redirection is used to pass output to either a file or stream whereas pipes are used to pass output to another program or utility.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > STDERR allows us to separate errors from output which allows us to see diagnostic information as well as output. Also when writing scripts, having an output be ignored while catching errors is vital.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > We should handle errors separately from stdout for clarity and error reporting. We should provide a way to merge them if the user wants to see outputs as well as errors, this can be done by redirecting both streams to the same location