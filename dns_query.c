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


/*
 * Adapt the domain format from usual format to dns format.
 * eg.)www.example.com -> \3www\6example\3com\0
 */
static char *adapt_domain_format(char *domain, char *adapted_domain)
{
    char *d = domain;
    char *ad= adapted_domain;

    while(1){
        char label_len = 0;
        char *label_start = d;
        while(!(*d == '\0' || *d == '.' )){
            d++;
            label_len++;
        }

        *ad++ = label_len;
        strncpy(ad, label_start, label_len);
        ad += label_len;

        if(*d)
            d++;
        else
            break;
    }
    *ad = '\0';

    return adapted_domain;
}


/*Call srand() before use*/
static unsigned int build_dns_query(char *domain, uint8_t *dns_query)
{
    uint8_t *qname;
    unsigned int question_offset_bytes;
    unsigned int dns_query_bytes;

    struct dns_hdr_t *dns_hdr;
    struct question_t *question;

    dns_hdr = (struct dns_hdr_t *)dns_query;

    dns_hdr->id = (uint16_t)rand();
    dns_hdr->qr = 0;
    dns_hdr->opcode = 0;
    dns_hdr->aa = 0;
    dns_hdr->tc = 0;
    dns_hdr->rd = 1;
    dns_hdr->ra = 0;
    dns_hdr->z = 0;
    dns_hdr->ad = 0;
    dns_hdr->cd = 0;
    dns_hdr->rcode = 0;
    dns_hdr->qdcount = htons(1);
    dns_hdr->ancount = 0;
    dns_hdr->nscount = 0;
    dns_hdr->arcount = 0;

    qname = (uint8_t *)(dns_query + sizeof(struct dns_hdr_t));
    adapt_domain_format(domain, (char *)qname);

    question_offset_bytes = sizeof(struct dns_hdr_t) + strlen((char *)qname) + 1;
    question = (struct question_t *)(dns_query + question_offset_bytes);
    question->type = htons(1);
    question->class = htons(1);

    dns_query_bytes = question_offset_bytes + sizeof(struct question_t);
    return dns_query_bytes;
}


void send_dns_query(char *domain, char *dns_server_ip, int sock, struct sockaddr_in dst_addr)
{
    uint8_t dns_query[1024];
    unsigned int dns_query_bytes;

    dns_query_bytes = build_dns_query(domain, dns_query);

    if(sendto(sock, dns_query, dns_query_bytes, 0, (struct sockaddr *)&dst_addr, sizeof(dst_addr)) < 0){
        perror("sendto");
    }
}


void recv_dns_response(int sock, struct sockaddr_in dst_addr)
{
    char buf[65536];
    unsigned int recv_bytes;
    socklen_t addrlen = sizeof(dst_addr);

    if((recv_bytes = 
                recvfrom(sock, (char*)buf, 65536 , 0 ,
                    (struct sockaddr*)&dst_addr , &addrlen)) < 0){
        perror("recvfrom");
    }

    hexdump((unsigned char *)buf, recv_bytes);
}


int main(int argc, char *argv[])
{
    int sock;
    char *dns_server_ip = "8.8.8.8";//Google DNS
    //char *dns_server_ip = "127.0.0.1";//loopback
    char *domain = "www.google.com";
    struct sockaddr_in dst_addr;

    if(argc == 2){
        domain = argv[1];
    }else if(argc > 2){
        printf("Input only one domain as a argument.\n");
        return 1;
    }

    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket");
    }

    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(53);
    inet_aton(dns_server_ip, &dst_addr.sin_addr);

    srand((unsigned)time(NULL));

    send_dns_query(domain, dns_server_ip, sock, dst_addr);

    recv_dns_response(sock, dst_addr);

    return 0;
}
