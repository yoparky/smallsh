#include "utilities.h"
#include <unistd.h>
#include "data_struct.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* parseIn
**************************************************************************************************
* Given an input into smallsh's prompt, parse it by tokenizing with a whitespace as the delimiter.
* While doing so, account for the comments and background option (&) setting. 
* Input:
*    struct Data *dp, holds the user input data that this function will fill out
*    int pid, holds the pid of the process. Used in $$ conversion
*    int *bg, holds the pointer to the background indicator, updated in this function
* Output:
*   void
*/
void parseIn(struct Data *dp, int pid, int* bg) {
    // worst case temporary buffer (max input requirement was 2048 characters)
    char buf[2049];
    fgets(buf, 2049, stdin);
    // remove newline at end from fgets
    buf[strcspn(buf, "\n")] = '\0';

    // if an empty string or comment, keep the first string in input as NULL
    if (buf[0] == '#' || buf[0] == '\0') {
        // the main loop catches if the first argument is NULL for comment handling
        return;
    }

    // strtok with whitespace as delim
    char *token = strtok(buf, " ");
    int i = 0;
    // fill out struct
    while (token != NULL) {
        // update input file
        if (!strcmp(token, "<")) {
            token = strtok(NULL, " ");
            strcpy(dp->filein, token);
        // update output file
        } else if (!strcmp(token, ">")) {
            token = strtok(NULL, " ");
            strcpy(dp->fileout, token);
        // we update background request...
        } else if (!strcmp(token, "&")) {
            token = strtok(NULL, " ");
            // only if & comes at the end
            if (token == NULL) {
                *bg = 1;
            }
        // all other arguments 
        } else {
            // copy down the token
            dp->input[i] = strdup(token);
            // update count for free call for strdup
            dp->inputcount += 1;
            // converting $$
            int pos = -1;
            int len = strlen(token);
            for (int k = 0; k < len; k++) {
                if (token[k] == '$') {
                    if (token[k+1] != '\0' && token[k+1] == '$') {
                        pos = k;
                        // custom function that inserts pid in place of $$
                        insertString(token, pid, pos, &(dp->input[i]));
                        k += 1;
                    }
                }
            }
            i += 1;
        }
        // check next token
        token = strtok(NULL, " ");
    }
}

/* insertString
**************************************************************************************************
* Given an original string, an int process id, and the position of the first $, insert the pid
* into the string in place of $$.
* 
* Input:
*    char *str, is the original string that contains ...$$...
*    int pid, holds the pid of the process. Converted to string to insert into $$
*    int pos, holds the index of the first instance of $ in the original string
*    int **target, holds the pointer to string token in data struct that will be directly updated
* Output:
*   void
*/
void insertString(char *str, int pid, int pos, char **target) {
    // max length of pid as buffer
    char pids[8];
    // convert to string
    snprintf(pids, 8, "%d", pid);
    int strl = strlen(str);
    int pidl = strlen(pids);
    // malloc space for the new string
    char *inserted = (char*)malloc(strl + pidl + 1);
    // copy up to $
    strncpy(inserted, str, pos);
    inserted[pos] = '\0';
    // insert pid string
    strcat(inserted, pids);
    // skip the second $ and copy the rest
    strcat(inserted, str + pos + 2);
    // update the target
    *target = inserted;
}
/* initStructData
**************************************************************************************************
* Initializes or cleans out struct data. Some elements that require tracking are not cleaned out
* (e.g. status)
* 
* Input:
*    struct Data *dp, holds the data to be cleaned out
* Output:
*   void
*/
void initStructData (struct Data *dp) {
    for (int i = 0; i < 512; i++) {
        dp->input[i] = NULL;
    }
    dp->filein[0] = '\0';
    dp->fileout[0] = '\0';
    dp->inputcount = 0;
}
/* freeInput
**************************************************************************************************
* Frees the malloc call from parseIn (from strdup and insertString). 
* 
* Input:
*    struct Data *dp, holds the user input data to be freed
* Output:
*   void
*/
void freeInput (struct Data *dp) {
    // using our count of input tokens, free up the input array
    while (dp->inputcount != 0) {
        free(dp->input[(dp->inputcount) - 1]);
        dp->inputcount -= 1;
    }
}

/* handleSIGTSTP
**************************************************************************************************
* Custom handler for SIGTSTP. Updates the bgNormalCondition (1 when background allowed, 0 when
* not allowed) and prints a message re-entrantly in accordance with the toggled mode.
* Input:
*   int signo
* Output:
*   void
*/
void handleSIGTSTP(int signo) {
    // bg on state
    if (bgNormalCondition == 1) {
        char *message = "\nEntering foreground-only mode (& is now ignored)\n";
        write(1, message, 50);
        fflush(stdout);
        bgNormalCondition = 0;
    // bg off state
    } else {
        char *message = "\nExiting foreground-only mode\n";
        write(1, message, 30);
        fflush(stdout);
        bgNormalCondition = 1;
    }
}
