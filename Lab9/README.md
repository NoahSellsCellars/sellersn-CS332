# Lab9

This program's main function is to:
1.  **Fork**: Create a new child process
2.  **Exec** The child process uses 'execvp' to replace itself with the <command> argument.
3. **Wait**: The parent process waits and manages the child.

## Features

* **Command Execution**: Runs any command specified on the command line.
* **Signal Forwarding**: The parent process catches signals and forwards them to the child.
    * `Ctrl+C` (SIGINT) is caught and sent to the child.
    * `Ctrl+Z` (SIGTSTP) is caught and sent to the child.
* **Parent Control**:
    * `Ctrl+\` (SIGQUIT) tells the parent wrapper to exit.
* **Child Cleanup**: The parent uses `SIGCHLD` and `waitpid` to "reap" the child process when it exits, preventing it from becoming a zombie.

## To Compile

```bash
gcc -Wall -O -o forkexecvp forkexecvp.c
```
## To Run

```bash
./forkexecvp <command> [args]
```
