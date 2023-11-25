#ifndef UTILITIES_H
#define UTILITIES_H

#include "data_struct.h"

void parseIn(struct Data *dp, int pid, int *bg);
void insertString(char *str, int pid, int pos, char **target);
void initStructData(struct Data *dp);
void freeInput(struct Data *dp);
void handleSIGTSTP(int signo);

#endif
