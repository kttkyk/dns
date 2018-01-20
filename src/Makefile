CC = gcc
CFLAGS = -Wall
BIN_DIR = ../bin/

dnsc_objs = dns_question.o dns_answer.o udp.o utils.o
test_udp_objs = test_udp.o udp.o utils.o
test_dns_objs = test_dns.o $(dnsc_objs)
hdrs = ../lib/dns.h ../lib/udp.h ../lib/utils.h

.PHONY: all

.SUFFIXES: .c .o

all: test_udp test_dns

test_udp: $(test_udp_objs)
	$(CC) -o $(BIN_DIR)$@ $^

test_dns: $(test_dns_objs)
	$(CC) -o $(BIN_DIR)$@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

dns_question.o: $(hdrs)

dns_answer.o: $(hdrs)

udp.o: $(hdrs)

utils.o: $(hdrs)

clean:
	rm *.o ../bin/*

