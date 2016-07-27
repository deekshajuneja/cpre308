#include <stdio.h>
#include <stdint.h>

inline uint16_t swp_end_16(uint16_t n)
{
 return (n << 8) | (n >> 8);
}

inline uint32_t swp_end_32(uint32_t n)
{
 return ((n << 24)  & 0xFF000000) | ((n << 8) & 0x00FF0000) | ((n >> 8) & 0x0000FF00) | ((n >> 24) & 0x000000FF);
}

int main(int argc, const char * argv[])
{
 uint32_t a = 0x12345678;
 uint8_t *c = (uint8_t*)(&a);
  if (*c == 0x78) {
   printf("little-endian\n");
  } else {
   printf("big-endian\n");
  }
 return 0;
}
