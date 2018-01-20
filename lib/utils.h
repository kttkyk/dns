#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

void hexdump(unsigned char *data, unsigned int data_bytes);
uint16_t checksum(uint8_t *data, unsigned int size);

#endif
