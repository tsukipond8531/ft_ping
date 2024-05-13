#ifndef ICMP_H_
#define ICMP_H_
#pragma once

#include <netdb.h>
#include <sys/time.h>

#define WORDS_TO_BYTES(words) words * 4
#define DATA_SIZE 56
#define ICMP_ECHO 8
#define ICMP_ECHO_REPLY 0

typedef struct __attribute__((__packed__)) s_icmp {
  uint8_t type;
  uint8_t code;
  uint16_t checksum;
  uint16_t identifier;
  uint16_t sequence;
  struct timeval time;
} t_icmp;

/*
 * Converts _icmp_ and _data_ to a byte array with the correct fields in
 * network format. Whilst the return value is the byte array, the resulting
 * size of the operation will be placed in _datalen_ if not NULL.
 *
 * When _datalen_ is not NULL but _data_ is, or the sizes do not match, the
 * behaviour is undefined.
 */
uint8_t *icmp_bytes(t_icmp icmp, uint8_t const *data, uint16_t *datalen);

/*
 * Converts a sequence of _bytes_ of length _len_ to a t_icmp. A verify
 * of the checksum is done and, if failed, returns 1.
 *
 * The _bytes_ are assumed to be received from a raw socket, so an initial
 * IP header is expected, which will be used to understand the structure
 * of the received information.
 *
 * All fields will be automatically converted to host format and network
 * format will be assumed.
 */
int icmp_from_bytes(t_icmp *icmp, uint8_t const *const bytes);

#endif // !ICMP_H_
