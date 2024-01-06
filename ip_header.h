#ifndef IP_HEADER_H_
#define IP_HEADER_H_
#pragma once

// A word is 32-bits. A byte is 8-bits. 5 words is 20 bytes
#define WORDS_TO_BYTES(x) x * 4

#include <stddef.h>
#include <stdint.h>

typedef enum e_ip_types_of_service {
  IP_PREC_NETWORK_CONTROL = 0xe0,
  IP_PREC_INET_CONTROL = 0xc0,
  IP_PREC_CRIT = 0xa0,
  IP_PREC_FLASH_OVERRIDE = 0x80,
  IP_PREC_OVERRIDE = 0x60,
  IP_PREC_IMMEDIATE = 0x40,
  IP_PREC_PRIORITY = 0x20,
  IP_DELAY_LOW = 0x10,
  IP_THROUGHPUT_HIGH = 0x08,
  IP_RELIABILITY_HIGH = 0x04,
} t_ip_types_of_service;

typedef enum e_ip_protocols {
  IP_ICMP = 0x01,
} t_ip_protocols;

/*
** IP Header
**
** Implementation of the RFC 791, taking in consideration endianness.
**
** /!\ ONLY IPv4 IS SUPPORTED /!\
**
** The setting of fields is only recommended through the respective
** setters, as the behaviour will be more natural and the checksum
** will be automatically computed.
**
** Options can be manually set and no enum is available to encode
** them. Refer to RFC 791 for further information. Rememe
** Fragmentation can be manually set and no enum is available to
** encode them. REfer to RFC 791 for further information.
*/
typedef struct __attribute__((packed)) s_ip_header {
#ifndef __BIG_ENDIAN
  uint8_t internet_header_length : 4;
  uint8_t version : 4;
#else
  uint8_t version : 4;
  uint8_t internet_header_length : 4;
#endif
  uint8_t type_of_service;
  uint16_t total_length;
  uint16_t identification;
  uint16_t fragmentation;
  uint8_t time_to_live;
  uint8_t protocol;
  uint16_t checksum;
  uint32_t source;
  uint32_t destination;
  uint32_t options;
} t_ip_header;

/*
** create_ip_header
**
** Allocated the IP header and creates an IP header for
** IPv4 with basic fields set
*/
t_ip_header *create_ip_header(void);
/*
** unpack_ip_header
**
** Allocates the IP header and creates an IP header based
** on the incoming packet. All fields are assumed to be
** in network format and converted to host format,
** including checksum.
**
** unpack_ip_header(pack_ip_header(unpack_ip_header(pack_ip_header(create_ip_header()))))
** Will generate leaks, but the resulting unpacked structure will be
** exactly the initial one
*/
t_ip_header *unpack_ip_header(void const *const bytes);
/*
** pack_ip_header
**
** Allocates an array of bytes containing the data of the IP
** header. The array is returned in network format with
** the checksum in network format as well
**
** unpack_ip_header(pack_ip_header(unpack_ip_header(pack_ip_header(create_ip_header()))))
** Will generate leaks, but the resulting unpacked structure will be
** exactly the initial one
*/
uint8_t *pack_ip_header(t_ip_header const *const ip);
/*
** set_internet_header_length
**
** Sets the IP header length dynamically according to the current
** content.
** /!\ Must be called specially if setting options /!\
*/
void set_internet_header_length(t_ip_header *const ip);
/*
** set_type_of_service
**
** Sets the type of service using the header t_ip_types_of_service.
** Set the type of service using the | operator:
** IP_PREC_CRIT | IP_DELAY_LOW | IP_RELIABILITY_HIGH
*/
void set_type_of_service(t_ip_header *const ip,
                         t_ip_types_of_service const tos);
/*
** set_body_length
**
** In bytes, sets the body length that will be attached to the
** header. The total_length will automatically be set according
** to the internet_header_length
*/
void set_body_length(t_ip_header *const ip, uint16_t const length);
/*
** set_identification
**
** Overwrites the beautiful identification set by default 0x4242
** and defines a new identification for the IP frame
*/
void set_identification(t_ip_header *const ip, uint16_t const id);
/*
** set_time_to_live
**
** Overrides the default 0xff (255) time to live and sets a new
** one for the given IP header
*/
void set_time_to_live(t_ip_header *const ip, uint8_t const ttl);
/*
** set_protocol
**
** Sets the protocol using the t_ip_protocols enum
*/
void set_protocol(t_ip_header *const ip, t_ip_protocols const protocol);
/*
** set_ip_header_checksum
**
** Recomputes the checksum for the IP frame, using for it
** the network byte order BUT leaving the checksum in host
** byte order until it's packed using pack_ip_header
*/
void set_ip_header_checksum(t_ip_header *const ip);
/*
** set_source
**
** Sets the source to be used by the IP frame
*/
void set_source(t_ip_header *const ip, uint32_t const source);
/*
** set_destination
**
** Sets the destination to be targetted by the IP frame
*/
void set_destination(t_ip_header *const ip, uint32_t const destination);

#endif // !IP_HEADER_H_
