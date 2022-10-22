#include "file.h"

FILE* openFile(char *name, char* mode){
    FILE *ret;
    ret = fopen(name, mode);
    if(ret == NULL){
        printf("ERROR OPENING FILE\n");
    }
    return ret;
}

int closeFile(FILE *file){
    return fclose(file);
}

int getFileSize(FILE *file){
    fseek(file, 0L, SEEK_END);
    int fileSize = (int)ftell(file);
    rewind(file);
    return fileSize;
}