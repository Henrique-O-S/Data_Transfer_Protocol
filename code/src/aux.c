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