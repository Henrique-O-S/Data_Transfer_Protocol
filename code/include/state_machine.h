#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "auxiliar.h"
#include "macros.h"

typedef enum state {
    START = 0,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP
} state_st;

typedef struct state_machine {
    state_st state;
    unsigned char* controlBytes;
    int controlBytesLength;
    unsigned char addressByte;
    int controlByteIndex;
    int dataLength;
} state_machine_st;

int isControlByte(unsigned char byte, state_machine_st* sm);

void change_state(state_machine_st* sm, state_st st);

state_machine_st* create_state_machine(unsigned char* controlBytes, int controlBytesLength, unsigned char addressByte);

void event_handler(state_machine_st* sm, unsigned char byte, unsigned char* frame, int mode);

void destroy_st(state_machine_st* sm);

#endif // _STATE_MACHINE_H_
