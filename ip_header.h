#ifndef IP_HEADER_H_
#define IP_HEADER_H_
#pragma once

#include <stddef.h>
#include <stdint.h>

typedef enum e_ip_versions {
  IP_V4 = 0x4,
} t_ip_versions;

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

typedef enum e_ip_fragmentation_flags {
  IP_FRAGMENT_DONT = 0x02,
  IP_FRAGMENT_MORE = 0x01,
} t_ip_fragmentation_flags;

typedef enum e_ip_protocols {
  IP_ICMP = 0x01,
} t_ip_protocols;

/*
** IP Header
**
** Implementation of the RFC 791, taking in consideration endianness.
**
** The setting of fields is only recommended through the respective
** setters, as the behaviour will be more natural and the checksum
** will be automatically computed.
**
** Options must be manually set and no enum is available to encode
** them. Refer to RFC 791 for further information.
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
  uint8_t fragmentation_flags : 3;
  uint16_t fragmentation_offset : 13;
  uint8_t time_to_live;
  uint8_t protocol;
  uint16_t checksum;
  uint32_t source;
  uint32_t destination;
  uint32_t options : 24;
  uint8_t padding : 8;
} t_ip_header;

/*
** create_ip_header
**
** Creates an IP header for IPv4 with basic fields set
*/
t_ip_header *create_ip_header(void);
/*
** unpack_ip_header
**
** Created an IP header based on an incoming packet.
** The checksum will be validated. In case of an invalid
** header, NULL will be returned
*/
t_ip_header *unpack_ip_header(void const *const bytes, size_t const number);
uint8_t *pack_ip_header(t_ip_header const *const ip);
void set_version(t_ip_header *const ip, t_ip_versions const version);
void set_internet_header_length(t_ip_header *const ip);
void set_type_of_service(t_ip_header *const ip,
                         t_ip_types_of_service const tos);
void set_total_length(t_ip_header *const ip, uint16_t const length);
void set_identification(t_ip_header *const ip, uint16_t const id);
void set_fragmentation_flags(t_ip_header *const ip,
                             t_ip_fragmentation_flags const flags);
void set_fragmentation_offset(t_ip_header *const ip, uint16_t offset);
void set_time_to_live(t_ip_header *const ip, uint8_t const ttl);
void set_protocol(t_ip_header *const ip, t_ip_protocols const protocols);
void set_checksum(t_ip_header *const ip);
void set_source(t_ip_header *const ip, uint32_t const source);
void set_destination(t_ip_header *const ip, uint32_t const destination);

#endif // !IP_HEADER_H_
