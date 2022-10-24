#ifndef _FRAME_H_
#define _FRAME_H_

#include "macros.h"
#include "auxiliar.h"
#include "state_machine.h"
#include "alarm.h"

int sendFrame(unsigned char* frame, int fd, int length);

int createIFrame(unsigned char* frame, unsigned char controlField, const unsigned char* dataField, int infoFieldLength);

int createSFrame(unsigned char* frame, unsigned char address, unsigned char control);

int readSupervisionFrame(unsigned char* frame, int fd, unsigned char* controlBytes, int controlBytesLength, unsigned char address);

int readSupervisionFrame(unsigned char* frame, int fd, unsigned char* controlBytes, int controlBytesLength, unsigned char address);

int readSFrame(unsigned char* frame, int fd, unsigned char* controlBytes, int controlBytesLength, unsigned char addressByte);

int readIFrame(unsigned char* frame, int fd, unsigned char* controlBytes, int controlBytesLength, unsigned char addressByte);

#endif // _FRAME_H_