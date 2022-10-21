#include "aux.h"


int(util_get_LSB)(int val, unsigned char *lsb) {
  *lsb = (unsigned char) val;
  return 0;
}

int(util_get_MSB)(int val, unsigned char *msb) {
  *msb = (unsigned char) (val >> 8);
  return 0;
}