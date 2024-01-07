#ifndef ICMP_H_
#define ICMP_H_
#pragma once

#include "ip_header.h"

#define DATA_SIZE 32

typedef enum e_icmp_types {
  ICMP_ECHO_REPLY = 0,
  ICMP_ECHO = 8,
} t_icmp_types;

typedef struct __attribute__((__packed__)) s_icmp {
  uint8_t type;
  uint8_t code;
  uint16_t checksum;
  union __attribute__((__packed__)) payload {
    struct __attribute__((__packed__)) s_echo {
      uint16_t identifier;
      uint16_t sequence;
      uint8_t data[DATA_SIZE];
    } echo;
  } payload;
} t_icmp;

/*
** create_icmp
**
** Allocates and creates an ICMP header with the given type.
** The set type will limit the possible fields to be set
**
** If the type is set to ICMP_ECHO or ICMP_ECHO_REPLY, the data
** will be left empty on DATA_SIZE values ASCII NUL (\0)
*/
t_icmp *create_icmp(t_icmp_types const type);

/*
** icmp_to_packet
**
** Allocates a fixed and packed version of the bytes between
** the IP header and the ICMP payload, making sure to set and
** verify the checksum if needed.
**
** All pertinent validations (such as the type ICMP in the
** IP header) will be checked before formatting the packet.
** If an invalid or incongruous value is found, the packet is
** discarded without the arguments being free and NULL is set.
**
** The result is a byte sequence that can be sent through a raw
** socket and will be understood as an ICMP packet
*/
uint8_t *icmp_to_packet(t_ip_header const *const header,
                        t_icmp const *const payload);

/*
** pack_icmp_payload
**
** Allocates an array of bytes containing the data of the ICMP
** structure. The array is returned in network format with
** the checksum in network format as well
**
** unpack_icmp_payload(pack_icmp_payload(unpack_icmp_payload(pack_icmp_payload(create_icmp(8)))))
** Will generate leaks, but the resulting unpacked structure will be
** exactly the initial one
*/
uint8_t *pack_icmp_payload(t_icmp const *const payload);

/*
** unpack_icmp_header_from_packet
**
** The input is expected to be a fully fledged packet, and the
** return value will be the deconstructed packet's header.
** If the checksum were to be invalid, or any other error or
** incongruence was found in the process, NULL would be
** returned instead.
**
** Wrapper of unpack_ip_header which will dynamically take
** strictly the necessary bytes
*/
t_ip_header *unpack_icmp_header_from_packet(void const *const bytes);

/*
** unpack_icmp_payload_from_packet
**
** The input is expected to be a fully fledged packet, and the
** return value will be the deconstructed packet's payload.
** If the checksum were to be invalid, or any other error or
** incongruence was found in the process, NULL would be
** returned instead
*/
t_icmp *unpack_icmp_payload_from_packet(void const *const bytes);

/*
** set_identifier
**
** If and only if the type of ICMP is ICMP_ECHO or ICMP_ECHO_REPLY,
** sets the ICMP identifier (not to be confused with header identifier).
**
** It's a value that will help link all packets given any sequence
*/
void set_identifier(t_icmp *const packet, uint16_t identifier);

/*
** set_sequence
**
** If and only if the type of ICMP is ICMP_ECHO or ICMP_ECHO_REPLY,
** sets the ICMP sequence.
**
** It's a value that will help understand which packet is being sent.
** The usage of set_sequence_more is recommended instead, as it will
** gradually increase the value
*/
void set_sequence(t_icmp *const packet, uint16_t const seq);

/*
** set_sequence_more
**
** If and only if the type of ICMP is ICMP_ECHO or ICMP_ECHO_REPLY,
** sets the ICMP sequence to the current plus one.
**
** It's a value that will help understand which packet is being sent
*/
void set_sequence_more(t_icmp *const packet);

/*
** set_data
**
** 32 bytes by default. Unless specified, it will default to 32 random
** bytes using the stack
*/
void set_data(t_icmp *const packet, uint8_t data[DATA_SIZE],
              uint8_t const data_size);

/*
** set_icmp_checksum
**
** Recomputes the checksum for the ICMP, using for it
** the network byte order BUT leaving the checksum in host
** byte order until it's packed using icmp_to_packet
*/
void set_icmp_checksum(t_icmp *const packet);

#endif // !ICMP_H_
