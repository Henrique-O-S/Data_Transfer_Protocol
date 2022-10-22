#ifndef _FRAME_H_
#define _FRAME_H_

#include "macros.h"
#include "aux.h"

int createIFrame(unsigned char* frame, unsigned char controlField, unsigned char* dataField, int infoFieldLength);

int createSFrame(unsigned char* frame, unsigned char address, unsigned char control);



#endif // _FRAME_H_