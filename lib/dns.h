#ifndef DNS_H
#define DNS_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


struct dns_hdr_t
{
    uint16_t id;

    uint8_t rd:1;
    uint8_t tc:1;
    uint8_t aa:1;
    uint8_t opcode:4;
    uint8_t qr:1;

    uint8_t rcode:4;
    uint8_t cd:1;
    uint8_t ad:1;
    uint8_t z:1;
    uint8_t ra:1;

    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
};


//question_t does not have qname,
//since qname has variable length.
struct question_t
{
    uint16_t type;
    uint16_t class;
};


//The rdata is for ip address only(4 bytes)
struct resource_t
{
    uint16_t name;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t rdlength;
    uint32_t rdata;
}__attribute((__packed__));


unsigned int build_question_section(char *domain, uint8_t *dns_question_section);
unsigned int build_dns_query(char *domain, uint8_t *dns_query);
void send_dns_query(char *domain, int sock, struct sockaddr_in dst_addr);
void send_spoofed_dns_query(char *domain, int raw_sock, struct sockaddr_in src_addr, struct sockaddr_in dst_addr);
void recv_dns_response(int sock, struct sockaddr_in dst_addr);
void send_spoofed_dns_response(uint16_t id, char *target_ip, char *target_domain, int raw_sock, struct sockaddr_in dst_addr);
unsigned int build_dns_response(uint16_t id, char *target_domain, char *target_ip, uint8_t *dns_response);
#endif

