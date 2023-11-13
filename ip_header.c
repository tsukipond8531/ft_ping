#include "ip_header.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

t_ip_header *create_ip_header(void) {
  t_ip_header *ip;

  ip = malloc(sizeof(t_ip_header));
  if (!ip) {
    printf("An error occurred whilst allocating memory");
    exit(1);
  }
  for (size_t i = 0; i < sizeof(t_ip_header); i++)
    *((uint8_t *)(ip + i)) = 0;
  ip->version = IP_V4;
  ip->internet_header_length = 0x05;
  ip->total_length = 0x05;
  ip->identification = 0x4242;
  ip->time_to_live = 0xff;
  return ip;
}

t_ip_header *unpack_ip_header(void const *const bytes, size_t const number) {
  t_ip_header *ip;
  uint8_t packet_header_length_32bit_words;
  uint16_t packet_header_checksum;

  packet_header_length_32bit_words = *((uint8_t *)bytes) & 0xff;
  return ip;
}

uint8_t *pack_ip_header(t_ip_header const *const ip) {
  uint8_t *bytes;

  bytes = malloc(sizeof(uint8_t) * ip->internet_header_length);
  if (!bytes) {
    printf("An error occurred whilst allocating memory");
    exit(1);
  }
}
