#include "conversions.h"
uint32_t ConvertFromSyncSafe(uint32_t x) {
  x = ((x & 0xff) << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | ((x & 0xff000000) >> 24);
  uint32_t a, b, c, d, x_final = 0x0;
  a = x & 0xff;
  b = (x >> 8) & 0xff;
  c = (x >> 16) & 0xff;
  d = (x >> 24) & 0xff;

  x_final = x_final | a;
  x_final = x_final | (b << 7);
  x_final = x_final | (c << 14);
  x_final = x_final | (d << 21);
  return x_final;
};