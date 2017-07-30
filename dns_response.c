#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

#include "dns.h"


static unsigned int build_dns_response(char *domain, uint16_t 


void send_dns_response(char *target_domain, int sock, struct sockaddr_in dst_addr)
{
}



int main(int argc, char *argv[])
{
    int sock;
    char *spoofed_src_ip = "192.168.0.1";
    char *target_domain = "www.google.com";
    char *target_ip = "127.0.0.1";
    struct sockaddr_in dst_addr;

    if(argc == 2){
        dst_port = htons((short)atoi(argv[1]));
    }else{
        printf("Input destination port");
    }

    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(dst_port);
    inet_aton(target_ip, &dst_addr.sin_addr);

    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket");
    }

    send_dns_response(target_domain, sock, dst_addr);

    return 0;
}

