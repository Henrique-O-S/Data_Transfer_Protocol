
#ifndef _FILE_H_
#define _FILE_H_

#include <stdio.h>

FILE* openFile(char *name, char* mode);

int closeFile(FILE *file);

int getFileSize(FILE *file);

#endif // _FILE_H_
