#include "frame.h"

int createIFrame(unsigned char* frame, unsigned char control, unsigned char* data, int dataSize) {

  frame[0] = FLAG;

  frame[1] = FIELD_A_T_INIT;

  frame[2] = control;

  frame[3] = headerBCC(FIELD_A_T_INIT, control);

  for(int i = 0; i < dataSize; i++) {
    frame[i + 4] = data[i];
  }

  frame[dataSize + 4] = dataBCC(data, dataSize);

  frame[dataSize + 5] = FLAG;

  return 0;
}

int createSFrame(unsigned char* frame, unsigned char address, unsigned char control) {

  frame[0] = FLAG;

  frame[1] = address;

  frame[2] = control;

  frame[3] = headerBCC(address, control);

  frame[4] = FLAG;

  return 0;
}