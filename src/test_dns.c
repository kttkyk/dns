#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>
#include <time.h>

#include "../lib/dns.h"
#include "../lib/utils.h"


#define MAX_BUF_BYTES 1024
#define GOOGLE_PUBLIC_DNS "8.8.8.8"
#define LOCALHOST "127.0.0.1"

void test_dns_question(void)
{
    unsigned char dns_query[MAX_BUF_BYTES];
    char *domain = "www.google.com";
    int sock, raw_sock;
    unsigned int query_bytes;
    struct sockaddr_in dst_addr;
    struct sockaddr_in spoofed_src_addr;

    printf("[+] Build DNS qeury...\n");
    query_bytes = build_dns_query(domain, dns_query);
    hexdump(dns_query, query_bytes);
    printf("\n\n");

    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(53);
    inet_aton(GOOGLE_PUBLIC_DNS, &dst_addr.sin_addr);

    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket");
        exit(1);
    }

    printf("[+] Send DNS query...\n");
    send_dns_query(domain, sock, dst_addr);


    spoofed_src_addr.sin_family = AF_INET;
    spoofed_src_addr.sin_port = htons(53);
    inet_aton("192.168.100.55", &spoofed_src_addr.sin_addr);

    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(9090);
    inet_aton(LOCALHOST, &dst_addr.sin_addr);

    if((raw_sock = socket(AF_INET, SOCK_RAW, IP_HDRINCL)) < 0){
        perror("socket");
        exit(1);
    }

    printf("[+] Send spoofed DNS query...\n\n");
    send_spoofed_dns_query(domain, raw_sock, spoofed_src_addr, dst_addr);

}


void test_dns_answer(void)
{
    unsigned short id = 0xabcd;
    char *domain = "kttkyk.com";
    char *ip = "192.168.100.1";
    unsigned char dns_response[MAX_BUF_BYTES];
    unsigned int dns_response_bytes;
    int raw_sock;
    struct sockaddr_in dst_addr;

    printf("[+] Build DNS response...\n");
    dns_response_bytes = build_dns_response(id, domain, ip, dns_response);

    hexdump(dns_response, dns_response_bytes);
    printf("\n\n");

    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(53);
    inet_aton(LOCALHOST, &dst_addr.sin_addr);

    if((raw_sock = socket(AF_INET, SOCK_RAW, IP_HDRINCL)) < 0){
        perror("socket");
        exit(1);
    }

    printf("[+] Send spoofed DNS response...\n\n");
    srand(time(NULL));
    send_spoofed_dns_response(id, ip, domain, raw_sock, dst_addr);
}


int main(void)
{
    test_dns_question();
    test_dns_answer();

    return 0;
}

