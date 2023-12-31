#include "frame.h"

extern int currentRetransmission, relay, stop;

int sendFrame(unsigned char* frame, int fd, int length) {

  int bytesWritten;
  if( (bytesWritten = write(fd, frame, length)) <= 0){
      return -1;
  }

  sleep(1);
  
  return bytesWritten;
}

int createIFrame(unsigned char* frame, unsigned char control, const unsigned char* data, int dataSize) {

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

int readSFrame(unsigned char* frame, int fd, unsigned char* controlBytes, int controlBytesLength, unsigned char addressByte) {

    state_machine_st *sm = create_state_machine(controlBytes, controlBytesLength, addressByte);

    unsigned char byte;
    while(sm->state != STOP && stop != TRUE && !relay) {
        if(readByte(&byte, fd) == 0){
          event_handler(sm, byte, frame, SUPERVISION);
        }
    }

    int ret = sm->controlByteIndex;

    destroy_st(sm);

    if(stop == TRUE || relay)
      return -1;

    return ret;

}

int readIFrame(unsigned char* frame, int fd, unsigned char* controlBytes, int controlBytesLength, unsigned char addressByte) {

    state_machine_st *sm = create_state_machine(controlBytes, controlBytesLength, addressByte);

    unsigned char byte;

    while(sm->state != STOP) {
        if(readByte(&byte, fd) == 0)
          event_handler(sm, byte, frame, INFORMATION);
    }

    int ret = sm->dataLength;

    destroy_st(sm);

    return ret + 5;

}