# msh — Minimal Unix Command-Line Shell

- **NAME:** CALEB KIPRUTO
- **Student ID:** SCT212-0066/2024

## Project Description
MSH is a simple command-line shell developed using the C programming language. It demonstrates the fundamental operations of a Unix-like operating system, particularly how processes are created, executed, and controlled.

The shell enables users to enter and execute commands, interpret command-line arguments, and launch external programs. It also provides functionality for executing built-in commands, running processes in the background, redirecting output to files, and handling basic system signals.


## How to Build and Run
Compile the shell using `make` and run the executable:

```bash
make
./msh
```
To remove compiled binaries and object files:

```bash
make clean
```

## Features Implemented

| Task | Feature | Status |
| :--- | :--- | :--- |
| 1 | Read input using `getline()` and exit cleanly on Ctrl-D | Complete |
| 2 | Split commands into arguments and ignore extra whitespace | Complete |
| 3 | Execute external commands using `fork()`, `execvp()` and `waitpid()` | Complete |
| 4 | Implement `cd`, `help` and `exit` | Complete |
| 5 | Display the previous command's exit status | Complete |
| 6 | Support background execution and reap completed jobs | Complete |
| 7 | Support output redirection using `>` | Complete |
| 8 | Handle Ctrl-C without terminating the shell | Complete |

## Design Explanation
The msh shell runs continuously by displaying a prompt, accepting commands, and checking the result of the previous command. It uses getline() to read what the user types and strtok() to separate the command into individual arguments.

When the user enters a command that is not built into the shell, msh uses fork() to create a child process to run it. The child process allows Ctrl+C to stop the running program. If the user uses > to redirect output, the shell uses open() and dup2() to send the output to a file. It then uses execvp() to execute the command. If the command fails to run, an error message is displayed and the child process exits with status 127.

The parent process waits for the command to finish using waitpid() and saves its exit status. If the command is interrupted by a signal, the shell records the signal-based exit status.

Some commands, such as cd and exit, are handled directly by the shell. This is because cd needs to change the shell's current directory, while exit needs to close the shell itself. If they were run as child processes, they would not affect the main shell.

Finally, when a background command (&) finishes, it can leave behind a zombie process. To prevent this, msh uses waitpid(-1, NULL, WNOHANG) to check for and clean up completed background processes without stopping the shell from accepting new commands.

## Known Limitations
Only single output redirection (>) is supported; input redirection (<), append redirection (>>), and pipes (|) are not implemented.

Quoted strings containing spaces (e.g., echo "hello world") are split on whitespace rather than treated as a single argument.

Maximum argument count per command is fixed at 64 tokens

## Sources and AI-Tool Declaration

- **Linux Manual Pages (`man`):** Used to check the syntax and flags for `fork()`, `execvp()`, `waitpid()`, `getline()`, `open()`, and `dup2()`.
- **AI-Tool Declaration:** Used Gemini for debugging compiler warnings, and organizing the Git commit workflow. All code was written, compiled, and tested on my local machine.