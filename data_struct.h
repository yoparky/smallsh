#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#define F_2_PI (float)(2. * M_PI)
#define NUMELEMENTS (1 * 1024 * 1024)
#define MAXPERIODS 100
#define BIGSIGNALFILEBIN (char *)"bigsignal.bin"
#define BIGSIGNALFILEASCII (char *)"bigsignal.txt"
#define CSVPLOTFILE (char *)"plot.csv"

struct Data {
    char *input[512];
    char filein[512];
    char fileout[512];
    int status;
    int inputcount;
};

extern int bgNormalCondition;

#endif
