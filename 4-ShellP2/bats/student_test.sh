#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

# Test 1
@test "dsh exits cleanly with 'exit' command" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

# Test 2: Execute a simple external command (e.g., 'ls')
@test "Execute external command 'ls' without errors" {
    run ./dsh <<EOF
ls
exit
EOF
    [ "$status" -eq 0 ]
}

# Test 3: Test empty input handling (should continue without error)
@test "Empty input line is handled correctly" {
    run ./dsh <<EOF

exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "" ]]  
}

# Test 4: Test 'cd' built-in command with a valid directory
@test "Change directory with 'cd' to existing directory" {
    run ./dsh <<EOF
cd /tmp
exit
EOF
    [ "$status" -eq 0 ]
}

# Test 5: Test 'cd' built-in command with no argument (should do nothing)
@test "cd with no argument does not crash" {
    run ./dsh <<EOF
cd
exit
EOF
    [ "$status" -eq 0 ]
}

# Test 6: Test invalid external command (should print error)
@test "Invalid external command prints error message" {
    run ./dsh <<EOF
not_exists
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "Error: Execution failure: No such file or directory" ]]
}

# Test 7: Test command with arguments (e.g., 'echo hello')
@test "Execute command with arguments 'echo hello'" {
    run ./dsh <<EOF
echo hello
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello" ]]
}

# Test 8: Test too many arguments (exceeding CMD_ARGV_MAX)
@test "Too many arguments results in error" {
    run ./dsh <<EOF
cmd arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9 arg10 arg11 arg12
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "error: piping limited to 9 commands" ]] 
}

# Test 9: Test quoted arguments (e.g., "hello world")
@test "Handle quoted arguments correctly" {
    run ./dsh <<EOF
echo "    hello world"
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "    hello world" ]]
}

# Test 10: Test EOF (Ctrl+D simulation) exits cleanly
@test "EOF input exits the shell cleanly" {
    run ./dsh <<EOF
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ $'\n' ]]  # Expect a newline on EOF
}