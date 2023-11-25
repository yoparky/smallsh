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

## Acknowledgements
This project is part of the coursework for Oregon State University, focusing on operating systems and Unix programming.

## Sample Execution

```bash
$ smallsh
: ls
junk   smallsh    smallsh.c
: ls > junk
: status
exit value 0
: cat junk
junk
smallsh
smallsh.c
: wc < junk > junk2
: wc < junk
       3       3      23
: test -f badfile
: status
exit value 1
: wc < badfile
cannot open badfile for input
: status
exit value 1
: badfile
badfile: no such file or directory
: sleep 5
^Cterminated by signal 2
: status &
terminated by signal 2
: sleep 15 &
background pid is 4923
: ps
  PID TTY          TIME CMD
 4923 pts/0    00:00:00 sleep
 4564 pts/0    00:00:03 bash
 4867 pts/0    00:01:32 smallsh
 4927 pts/0    00:00:00 ps
:
: # that was a blank command line, this is a comment line
:
background pid 4923 is done: exit value 0
: # the background sleep finally finished
: sleep 30 &
background pid is 4941
: kill -15 4941
background pid 4941 is done: terminated by signal 15
: pwd
/nfs/stak/users/chaudhrn/CS344/prog3
: cd
: pwd
/nfs/stak/users/chaudhrn
: cd CS344
: pwd
/nfs/stak/users/chaudhrn/CS344
: echo 4867
4867
: echo $$
4867
: ^C^Z
Entering foreground-only mode (& is now ignored)
: date
 Mon Jan  2 11:24:33 PST 2017
: sleep 5 &
: date
 Mon Jan  2 11:24:38 PST 2017
: ^Z
Exiting foreground-only mode
: date
 Mon Jan  2 11:24:39 PST 2017
: sleep 5 &
background pid is 4963
: date
 Mon Jan 2 11:24:39 PST 2017
: exit
$


