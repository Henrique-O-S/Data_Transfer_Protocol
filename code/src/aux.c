#include "aux.h"


int util_get_LSB (int val, unsigned char *lsb) {
  *lsb = (unsigned char) val;
  return 0;
}

int util_get_MSB (int val, unsigned char *msb) {
  *msb = (unsigned char) (val >> 8);
  return 0;
}

int util_join_bytes (int *ret, unsigned char msb, unsigned char lsb) {
  /* ret = (msb << 8) | lsb; */
  *ret = msb;
  *ret = *ret << 8;
  *ret = *ret | lsb;
  return 0;
}

int get_size_in_bytes (int fileSize, int *byteCount){
  *byteCount = fileSize / BYTE_SIZE;
  if(fileSize % BYTE_SIZE > 0){
    *byteCount++;
  }
  return 1;
}

unsigned char headerBCC(unsigned char address, unsigned char control){
  return address ^ control;
}

unsigned char dataBCC(unsigned char *data, int dataSize){
  unsigned char dataInit = data[0];
  for(int i = 1; i < dataSize; i++){
    dataInit = dataInit ^ data[i];
  }
  return dataInit;
}

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
