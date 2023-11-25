# smallsh - Custom Shell in C

## Introduction

`smallsh` is a custom shell written in C that implements a subset of features from well-known shells like bash. The custom shell focuses on Unix process API, signal handling, and I/O redirection.

## Features

### Command Prompt

- Uses `:` as the command prompt.
- Supports commands with arguments, input/output redirection, and background execution.
- Recognizes special symbols `<`, `>`, and `&`.
- Background command execution: the last word must be &. If the & character appears anywhere else, it is treated as normal text.
- Input or output redirection: the > or < words followed by a filename word must appear after all the arguments. Input redirection can appear before or after output redirection.
- 

### Comments and Blank Lines

- Lines starting with `#` are treated as comments.
- Blank lines do not trigger any action.

### Variable Expansion

- Expands instances of `$$` to the process ID of the `smallsh` shell.

### Built-in Commands

- **exit**: Exits the shell, terminating any child processes.
- **cd**: Changes the working directory.
- **status**: Displays the exit status or terminating signal of the last foreground process.

### Executing Other Commands

- Uses `fork()`, `exec()`, and `waitpid()` to execute non-built-in commands.

### Input & Output Redirection

- Handles input/output redirection using `dup2()`.

### Foreground & Background Execution

- Executes commands in the foreground by default.
- Supports background execution with the `&` symbol.

### Signal Handling

- **SIGINT (Ctrl+C)**: Ignored by the shell, terminates foreground child processes.
- **SIGTSTP (Ctrl+Z)**: Toggles foreground-only mode.

## Compilation and Usage
To compile smallsh, use the provided Makefile with the command:
- make smallsh
Run the shell with:
- ./smallsh

## Sample Execution

```bash
$ smallsh
: ls
mydir smallsh smallsh.c
: ls > mydir
: cat < mydir
smallsh smallsh.c
: # This is a comment
: exit
$


