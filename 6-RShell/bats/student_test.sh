#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "E1: Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

# Test 1
@test "T1: dsh exits cleanly with 'exit' command" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

# Test 2: Execute a simple external command (e.g., 'ls')
@test "T2: Execute external command 'ls' without errors" {
    run ./dsh <<EOF
ls
exit
EOF
    [ "$status" -eq 0 ]
}

# Test 3: Test empty input handling (should continue without error)
@test "T3: Empty input line is handled correctly" {
    run ./dsh <<EOF

exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "" ]]  
}

# Test 4: Test 'cd' built-in command with a valid directory
@test "T4: Change directory with 'cd' to existing directory" {
    run ./dsh <<EOF
cd /tmp
exit
EOF
    [ "$status" -eq 0 ]
}

# Test 5: Test 'cd' built-in command with no argument (should do nothing)
@test "T5: cd with no argument does not crash" {
    run ./dsh <<EOF
cd
exit
EOF
    [ "$status" -eq 0 ]
}

# Test 6: Test invalid external command (should print error)
@test "T6: Invalid external command prints error message" {
    run ./dsh <<EOF
not_exists
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "execvp: No such file or directory" ]]
}

# Test 7: Test command with arguments (e.g., 'echo hello')
@test "T7: Execute command with arguments 'echo hello'" {
    run ./dsh <<EOF
echo hello
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello" ]]
}

# Test 8: Test too many arguments (exceeding CMD_ARGV_MAX)
@test "T8: Too many arguments results in error" {
    run ./dsh <<EOF
cmd arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9 arg10 arg11 arg12
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "error: too many arguments, limit is 9" ]] 
}

# Test 9: Test quoted arguments (e.g., "hello world")
@test "T9: Handle quoted arguments correctly" {
    run ./dsh <<EOF
echo "    hello world"
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "    hello world" ]]
}

# Test 10: Test EOF (Ctrl+D simulation) exits cleanly
@test "T10: EOF input exits the shell cleanly" {
    run ./dsh <<EOF
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ $'\n' ]]  # Expect a newline on EOF
}

### PIPE TESTING ###

# Pipe 1: Basic single pipe functionality
@test "P1: Single pipe works correctly (ls | grep)" {
    run ./dsh <<EOF
ls | grep ".c"
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ ".c" ]]  # Should see .c files in output
}

# Pipe 2: Multiple pipes (three commands)
@test "P2: Multiple pipes work correctly (ls | grep | wc)" {
    run ./dsh <<EOF
ls | grep ".c" | wc -l
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "4" ]]  # Should output 4 (4 .c files)
}

# Pipe 3: Too many pipes (exceeding CMD_MAX)
@test "P3: Exceeding maximum pipes shows error" {
    run ./dsh <<EOF
ls | grep a | wc | cat | echo | grep b | sort | head | tail
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "error: piping limited to 8 commands" ]]
}

# Pipe 4: Pipe with built-in command fails gracefully
@test "P4: Mixing built-in cd with pipe shows error" {
    run ./dsh <<EOF
ls | cd /tmp
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "execvp: No such file or directory" ]]  # Should show execution error for cd
}

# Pipe 5: Empty pipe handling
@test "P5: Empty pipe command (ls |) shows warning" {
    run ./dsh <<EOF
ls | 
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "warning: no commands provided" ]]
}

# Pipe 6: Pipe with arguments
@test "P6: Pipe with command arguments works (echo | grep)" {
    run ./dsh <<EOF
echo "hello world" | grep "hello"
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello world" ]]
}

# Pipe 7: Multiple pipes with arguments
@test "P7: Multiple pipes with arguments (echo | grep | wc)" {
    run ./dsh <<EOF
echo "test line one" | grep "line" | wc -w
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "3" ]]  # Should count 3 words
}

# Pipe 8: Invalid command in pipe chain
@test "P8: Invalid command in pipe chain shows error" {
    run ./dsh <<EOF
ls | nonexistentcmd | wc
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "Error: Execution failure" ]]
}

# Pipe 9: Pipe output matches expected format
@test "P9: Pipe output format matches sample (ls | grep)" {
    run ./dsh <<EOF
ls | grep ".c"
exit
EOF
    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="dsh_cli.cdshlib.crsh_cli.crsh_server.clocalmodedsh4>dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "C1: Connect to server and run 'ls'" {
    run ./dsh 127.0.0.1 1234 <<EOF
ls
exit
EOF
    [ "$status" -eq 0 ]  # Client should exit successfully
}

@test "C2: Change directory with 'cd' and verify with 'pwd'" {
    run ./dsh 127.0.0.1 1234 <<EOF
cd bats
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "/home/brandonshort/CS283/6-RShell/bats" ]]  # Verify the directory changed to bats
}

@test "C3: Execute 'exit' command and check client disconnection" {
    run ./dsh 127.0.0.1 1234 <<EOF
exit
EOF
    [ "$status" -eq 0 ]  # Client should exit with no errors
}

@test "C4: Execute 'stop-server' and verify server termination" {
    run ./dsh 127.0.0.1 1234 <<EOF
stop-server
EOF
    [ "$status" -eq 0 ]  # Client should exit successfully
    ! ps -p $SERVER_PID > /dev/null  # Server process should be terminated
}

@test "C5: Test pipeline command execution" {
    run ./dsh 127.0.0.1 1234 <<EOF
ls | grep ".c"
exit
EOF
    [ "$status" -eq 0 ]
}

@test "C6: Test error handling for invalid command" {
    run ./dsh 127.0.0.1 1234 <<EOF
invalid_command
exit
EOF
    [ "$status" -eq 0 ]  # Client should still exit cleanly
    [[ "$output" =~ "execvp: No such file or directory" ]]  # Expect an error message (adjust based on actual implementation)
}