#include "commands.h"
#include "utilities.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int bgNormalCondition = 1;

int main() {
    // termination condition for loop. Updates on exit
    int exitCmd = 0;
    // initialize struct
    struct Data data;
    int pid = getpid();
    initStructData(&data);
    // manually updated to keep track of previous status
    data.status = 0;
    // a variable to store [&]
    int background = 0;

    // Signal handling
    struct sigaction SIGINT_action = {0};
    struct sigaction SIGTSTP_action = {0};
    // SIGINT is ignored
    SIGINT_action.sa_handler = SIG_IGN;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = 0;
    sigaction(SIGINT, &SIGINT_action, NULL);
    // SIGTSTP is redirected to a custom handler
    SIGTSTP_action.sa_handler = handleSIGTSTP;
    sigfillset(&SIGTSTP_action.sa_mask);
    SIGTSTP_action.sa_flags = 0;
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);
    
    while (!exitCmd) {
        // prompt user
        printf(": ");
        fflush(stdout);

        // parse user input
        parseIn(&data, pid, &background);
        // handle cases:
        // "" or # 
        if (data.input[0] == NULL) {
            continue;
        // exit
        } else if (!strcmp(data.input[0], "exit")) {
            exitCmd = 1;
        // cd
        } else if (!strcmp(data.input[0], "cd")) {
            caseCd(&data);
        // status
        } else if (!strcmp(data.input[0], "status")) {
            caseStatus(&data);
        // if none match, fork to exec
        } else {
            forkExec(&data, SIGINT_action, &background);
        }
        background = 0;
        // free malloc done in parseIn
        freeInput(&data);
        // clean up used data
        initStructData(&data);
        
    }
    return 0;
}
