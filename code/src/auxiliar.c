#include "auxiliar.h"


int util_get_LSB (int val, unsigned char *lsb) {
  *lsb = (unsigned char) val;
  return 0;
}

int util_get_MSB (int val, unsigned char *msb) {
  *msb = (unsigned char) (val >> 8);
  return 0;
}

int util_join_bytes (int *ret, unsigned char msb, unsigned char lsb) {
  *ret = msb;
  *ret = *ret << 8;
  *ret = *ret | lsb;
  return 0;
}

int get_size_in_bytes (int fileSize, int *byteCount){
  *byteCount = fileSize / BYTE_SIZE;
  if(fileSize % BYTE_SIZE > 0){
    byteCount++;
  }
  return 1;
}

unsigned char headerBCC(unsigned char address, unsigned char control){
  return address ^ control;
}

unsigned char dataBCC(const unsigned char *data, int dataSize){
  unsigned char dataInit = data[0];
  for(int i = 1; i < dataSize; i++){
    dataInit = dataInit ^ data[i];
  }
  return dataInit;
}

/*
int getAddressField (unsigned char *addressField, LinkLayerRole role, unsigned char controlField){
  
  if(controlField == UA || controlField == RR0 || controlField == RR1 || controlField == REJ0 || controlField == REJ1){
    if(role == LlTx){ // if response from transmiter
      *addressField = FIELD_A_R_INIT;
    }
    else{ // if response from receiver
      *addressField = FIELD_A_T_INIT;
    }
  }
  else if(controlField == SET || controlField == DISC){
    if(role == LlTx){ // if command from transmitter
      *addressField = FIELD_A_T_INIT;
    }
    else{ // if command from receiver
      *addressField = FIELD_A_R_INIT;
    }
  }
  else{
    return 1;
  }
  return 0;
}
*/


int stuffIFrame (unsigned char *frame, int frameSize){
  unsigned char cpy[frameSize];
  int flagByteNo = frameSize - 1;
  int shift = 0;

  for(int i = 0; i < frameSize; i++){
    cpy[i] = frame[i];
  }

  for(int i = 4; i < frameSize + shift; i++){  // only stuff data field of I FRAME
    if(cpy[i] == FLAG && i != flagByteNo){
      frame[i] = FLAG_STUF1;
      frame[i+1] = FLAG_STUF2;
      shift++;
    }
    else if(cpy[i] == ESC_BYTE){
      frame[i] = ESC_BYTE_STUF1;
      frame[i+1] = ESC_BYTE_STUF2;
      shift++;
    }
    else{
      frame[i + shift] = cpy[i];
    }
  }
  return frameSize + shift; // return new len

}

int unstuffIFrame (unsigned char *frame, int frameSize){
  unsigned char cpy[frameSize];
  int flagByteNo = frameSize - 2;
  int shift = 0;

  for(int i = 0; i < frameSize; i++){
    cpy[i] = frame[i];
  }

  for(int i = 4; i < frameSize + shift; i++){  // only stuff data field of I FRAME
    if(cpy[i] == FLAG_STUF1 && cpy[i+1] == FLAG_STUF2 && i != flagByteNo){
      frame[i] = FLAG;
      shift--;
      i++;
    }
    else if(cpy[i] == ESC_BYTE_STUF1 && cpy[i+1] == ESC_BYTE_STUF2){
      frame[i] = ESC_BYTE;
      shift--;
      i++;
    }
    else{
      frame[i + shift] = cpy[i];
    }
  }
  return frameSize + shift;   // return new len
}

int readByte(unsigned char* byte, int fd) {

    if(read(fd, byte, sizeof(unsigned char)) <= 0)
        return -1;

    return 0;
}

void alarmHandlerInstaller() {
    struct sigaction action;
    action.sa_handler = alarmHandler;

    if(sigemptyset(&action.sa_mask) == -1){
      perror("sigemptyset");
      exit(-1);
    }

    action.sa_flags = 0;

    if(sigaction(SIGALRM, &action, NULL) != 0){
      perror("sigaction");
      exit(-1);
    }
}

int getSequenceNumber (int number){
  return number % 256;
}
