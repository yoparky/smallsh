#include "commands.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

/* caseCd
**************************************************************************************************
* Called when cd is entered into prompt. If no directory is specified, return to HOME.
* If a directory is specified, attempt to change directory via chdir
* Input:
*   struct Data *dp, holds the user input parsed by parseIn
* Output:
*   void
*/
void caseCd(struct Data *dp) {
    // No directory entered, go HOME
    if (!dp->input[1]) {
        chdir(getenv("HOME"));
    } else {
    // Attempt to chdir into the provided directory
        int try = chdir(dp->input[1]);
        if (try == -1) {
            perror("No such directory");
            fflush(stderr);
        }
    }
}

/* caseStatus
**************************************************************************************************
* Called when status is entered into prompt. The function checks the status number stored in
* struct, check for normal / abnormal termination and prints a message including the
* exit status accordingly.
* Input:
*    struct Data *dp, holds the user input parsed by parseIn
* Output:
*   void
*/
void caseStatus(struct Data *dp) {
    // normal termination
    if (WIFEXITED(dp->status)) {
        printf("exit value %d\n", WEXITSTATUS(dp->status));
        fflush(stdout);
    // abnormal termination WIFSIGNALED
    } else {
        printf("terminated by signal %d\n", WTERMSIG(dp->status));
        fflush(stdout);
    }
}
/* forkExec
**************************************************************************************************
* Called when there is no match for custom implemented commands. Takes the command, fork it to 
* an execvp call.
* Input:
*    struct Data *dp, holds the user input parsed by parseIn
*    struct sigaction sa, holds the SIGINT sigaction
*    int *bg, holds the pointer to the background indicator
* Output:
*   void
*/
void forkExec(struct Data *dp, struct sigaction sa, int *bg) {
    // Unaccounted command forked
    pid_t spawnPid = fork();
    switch(spawnPid) {
        case -1:
            perror("failed to fork");
            fflush(stderr);
            exit(1);
            break;
        case 0:
        // In the child process
            // For a child, respect SIGINT as required
            sa.sa_handler = SIG_DFL;
            sigaction(SIGINT, &sa, NULL);
            // Before execute, handle the IO redirection
            int newinfd;
            int newoutfd;
            // input
            if (!(dp->filein[0] == '\0')) {
                int newinfd = open(dp->filein, O_RDONLY);
                if (newinfd == -1) {
                    perror("failed to open in input redirection");
                    fflush(stderr);
                    exit(1);
                }
                // dup to assign
                int result = dup2(newinfd, 0);
                if (result == -1) {
                    perror("failed to dup2 in input redirection");
                    fflush(stderr);
                    exit(2);
                }
                // close fd on exec
                fcntl(newinfd, F_SETFD, FD_CLOEXEC);
            // if background is allowed and we are in a background process, and 
            // no specified input redirect, send to dev/null.
            } else if (bgNormalCondition && *bg) {
                int newinfd = open("/dev/null", O_RDONLY);
                if (newinfd == -1) {
                    perror("failed to open in input redirection to dev/null");
                    fflush(stderr);
                    exit(3);
                }
                int result = dup2(newinfd, 0);
                if (result == -1) {
                    perror("failed to dup2 in input redirection, dev/null");
                    fflush(stderr);
                    exit(2);
                }
                // close fd on exec
                fcntl(newinfd, F_SETFD, FD_CLOEXEC);
            }
            // output
             if (!(dp->fileout[0] == '\0')) {
                int newoutfd = open(dp->fileout, O_WRONLY | O_TRUNC | O_CREAT, 0644);
                if (newoutfd == -1) {
                    perror("failed to open in output redirection");
                    fflush(stderr);
                    exit(1);
                }
                // dup to assign
                int result = dup2(newoutfd, 1);
                if (result == -1) {
                    perror("failed to dup2 in output redirection");
                    fflush(stderr);
                    exit(2);
                }
                // close fd on exec
                fcntl(newoutfd, F_SETFD, FD_CLOEXEC);
            // if background and no output redirect input, redir to dev/null
            } else if (bgNormalCondition && *bg) {
                int newoutfd = open("/dev/null", O_WRONLY | O_TRUNC | O_CREAT, 0644);
                if (newoutfd == -1) {
                    perror("failed to open in input redirection to dev/null");
                    fflush(stderr);
                    exit(3);
                }
                int result = dup2(newoutfd, 0);
                if (result == -1) {
                    perror("failed to dup2 in output redirection, dev/null");
                    fflush(stderr);
                    exit(2);
                }
                // close fd on exec
                fcntl(newoutfd, F_SETFD, FD_CLOEXEC);
            }

            // execvp using data stored in structure
            execvp(dp->input[0], dp->input);
                // execvp only returns on failure
                printf("%s: No such file or directory\n", dp->input[0]);
                fflush(stdout);
                exit(1);
            break;
            
        default:
        // In the parent process
            // in background, only when bg global is true and bg local flag is true
            if (bgNormalCondition == 1 && *bg == 1) {
                // in background, flag WNOHANG so that waitpid is non-blocking
                pid_t childPid = waitpid(spawnPid, &(dp->status), WNOHANG);
                printf("background pid is %d\n", spawnPid);
                fflush(stdout);
            // not in background. No flags.
            } else {
                pid_t childPid = waitpid(spawnPid, &(dp->status), 0);
            }
            /* -1 as argument - waits for any child process, returns immediately if no child exited.
             * returns: -1 on error, 0 when no child has yet changed state, positive indicates
             * while loop idea highly inspired by: 
            */ https://stackoverflow.com/questions/11322488/how-to-make-sure-that-waitpid-1-stat-wnohang-collect-all-children-process
            while ((spawnPid = waitpid(-1, &(dp->status), WNOHANG)) > 0) {
                // all children reaped
                printf("child pid %d is done: ", spawnPid);
                fflush(stdout);
                caseStatus(dp);
            }
    }
}
