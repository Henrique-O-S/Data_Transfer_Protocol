#ifndef _AUXILIAR_H_
#define _AUXILIAR_H_

#include <stdlib.h>
#include <signal.h>

#include "alarm.h"

int util_get_LSB (int val, unsigned char *lsb);

int util_get_MSB (int val, unsigned char *msb);

int util_join_bytes (int *ret, unsigned char msb, unsigned char lsb);

int get_size_in_bytes (int fileSize, int *byteCount);

unsigned char headerBCC (unsigned char address, unsigned char control);

unsigned char dataBCC (const unsigned char *data, int dataSize);

//int getAddressField (unsigned char *addressField, LinkLayerRole role, unsigned char controlField);

int stuffIFrame (unsigned char *frame, int frameSize);

int unstuffIFrame (unsigned char *frame, int frameSize);

int readByte(unsigned char* byte, int fd);

void alarmHandlerInstaller();

int getSequenceNumber (int number);

#endif // _AUXILIAR_H_
