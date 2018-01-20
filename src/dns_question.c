#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/if_ether.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

#include "../lib/dns.h"
#include "../lib/udp.h"
#include "../lib/utils.h"


/*
 * Adapt the domain format from usual format to dns format.
 * eg.)www.example.com -> \x3www\x6example\x3com\x0
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


unsigned int build_question_section(char *domain, uint8_t *dns_question_section)
{
    uint8_t *qname;
    unsigned int qtype_offset_bytes;
    unsigned int question_section_bytes;

    struct question_t *question;

    qname = dns_question_section;
    adapt_domain_format(domain, (char *)qname);

    //+ 1: To count the NULL byte at the end of qname
    qtype_offset_bytes = strlen((char *)qname) + 1;
    question = (struct question_t *)(dns_question_section + qtype_offset_bytes);
    question->type = htons(1);
    question->class = htons(1);

    question_section_bytes = qtype_offset_bytes + sizeof(struct question_t);
    return question_section_bytes;
}


/*Call srand() before use*/
unsigned int build_dns_query(char *domain, uint8_t *dns_query)
{
    uint8_t *qname;
    unsigned int question_section_bytes;
    unsigned int dns_query_bytes;

    struct dns_hdr_t *dns_hdr;

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
    question_section_bytes = build_question_section(domain, qname);

    dns_query_bytes = sizeof(struct dns_hdr_t) + question_section_bytes;
    return dns_query_bytes;
}


#define MAX_DNS_QUERY_LEN 1024

void send_dns_query(char *domain, int sock, struct sockaddr_in dst_addr)
{
    uint8_t dns_query[MAX_DNS_QUERY_LEN];
    unsigned int dns_query_bytes;

    dns_query_bytes = build_dns_query(domain, dns_query);

    if(sendto(sock, dns_query, dns_query_bytes, 0, (struct sockaddr *)&dst_addr, sizeof(dst_addr)) < 0){
        perror("sendto");
    }
}


void send_spoofed_dns_query(char *domain, int raw_sock, struct sockaddr_in src_addr, struct sockaddr_in dst_addr)
{
    uint8_t dns_query[MAX_DNS_QUERY_LEN];
    unsigned int dns_query_bytes;

    dns_query_bytes = build_dns_query(domain, dns_query);
    send_udp_packet(raw_sock, src_addr, dst_addr, dns_query, dns_query_bytes);
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

