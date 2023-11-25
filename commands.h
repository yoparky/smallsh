#ifndef COMMANDS_H
#define COMMANDS_H

#include "data_struct.h"
#include <signal.h>

void caseCd(struct Data *dp);
void caseStatus(struct Data *dp);
void forkExec(struct Data *dp, struct sigaction sa, int *bg);

#endif
