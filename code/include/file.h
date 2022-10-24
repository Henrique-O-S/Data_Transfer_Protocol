
#ifndef _FILE_H_
#define _FILE_H_

#include <stdio.h>
#include "macros.h"

FILE* openFile(const char *name, char* mode);

int closeFile(FILE *file);

int getFileSize(FILE *file);

int readBytesFromFile(FILE *file, unsigned char* dest);

int writeBytesToFile(FILE *file, unsigned char* source, int len);

#endif // _FILE_H_
