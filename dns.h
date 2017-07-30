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


struct resource_t
{
    uint16_t name;
    uint16_t type;
    uint16_t class;
    uint16_t ttl;
    uint16_t rdlength;
    uint8_t *rdata_p;
};
