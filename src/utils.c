#include <stdio.h>
#include <stdint.h>
#include <ctype.h>



void hexdump(unsigned char *data, unsigned int data_bytes)
{
    int bin_p, ascii_p;

    bin_p = ascii_p = 0;

    while(bin_p < data_bytes){
        int j;
        int whitespaces;
        for(j = 0; j < 8 && bin_p < data_bytes; j++){
            printf("%02x ", data[bin_p++]);
        }

        whitespaces = (8 - j) * 3;
        for(j = 0; j < whitespaces; j++){
            printf(" ");
        }

        for(j = 0; j < 8 && ascii_p < data_bytes; j++){
            if(isprint(data[ascii_p])){
                printf("%c", data[ascii_p++]);
            }else{
                printf(".");
                ascii_p++;
            }
        }

        printf("\n");
    }
}


uint16_t checksum(uint8_t *data, unsigned int size)
{
    int i;
    int sum = 0;
    uint16_t *p = (uint16_t *)data;

    for(i = 0; i < size; i += 2){
        sum += *(p++);
    }

    uint16_t carry = sum >> 16;
    uint16_t tmp = 0x0000ffff & sum;
    uint16_t res = ~(tmp + carry);

    return res;
}

