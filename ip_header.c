#include "ip_header.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

t_ip_header *create_ip_header(void) {
  t_ip_header *ip;

  ip = malloc(sizeof(t_ip_header));
  if (!ip) {
    printf("An error occurred whilst allocating memory\n");
    exit(1);
  }
  for (size_t i = 0; i < sizeof(t_ip_header); i++)
    *((uint8_t *)(ip + i)) = 0;
  ip->version = 0x04;
  ip->internet_header_length = 0x05;
  ip->total_length = 20;
  ip->identification = 0x4242;
  ip->time_to_live = 0x40;
  set_checksum(ip);
  return ip;
}

t_ip_header *unpack_ip_header(void const *const bytes) {
  t_ip_header *ip;
  uint8_t packet_ihl_words;

  packet_ihl_words = *(uint8_t *)bytes & 0x0f;
  if (!is_valid_checksum(bytes, WORDS_TO_BYTES(packet_ihl_words))) {
    return NULL;
  }
  ip = malloc(sizeof(t_ip_header));
  if (!ip) {
    printf("An error occurred whilst allocating memory\n");
    exit(1);
  }
  if (WORDS_TO_BYTES(packet_ihl_words) > sizeof(t_ip_header)) {
    printf("Attempted to unpack an IP header of size too large\n");
    exit(1);
  }
  ft_memcpy(ip, bytes, WORDS_TO_BYTES(packet_ihl_words));
  ip->total_length = ntohs(ip->total_length);
  ip->identification = ntohs(ip->identification);
  ip->fragmentation = ntohs(ip->fragmentation);
  ip->checksum = ntohs(ip->checksum);
  ip->source = ntohl(ip->source);
  ip->destination = ntohl(ip->destination);
  ip->options = ntohl(ip->options);
  return ip;
}

uint8_t *pack_ip_header(t_ip_header const *const ip) {
  t_ip_header copy;
  void *bytes;

  copy = *ip;
  copy.total_length = htons(ip->total_length);
  copy.identification = htons(ip->identification);
  copy.fragmentation = htons(ip->fragmentation);
  copy.checksum = htons(ip->checksum);
  copy.source = htonl(ip->source);
  copy.destination = htonl(ip->destination);
  copy.options = htonl(ip->options);

  bytes = malloc(sizeof(uint8_t) * WORDS_TO_BYTES(ip->internet_header_length));
  if (!bytes) {
    printf("An error occurred whilst allocating memory\n");
    exit(1);
  }
  ft_memcpy(bytes, &copy, WORDS_TO_BYTES(ip->internet_header_length));
  return bytes;
}

void set_checksum(t_ip_header *const ip) {
  void *bytes;

  ip->checksum = 0;
  bytes = pack_ip_header(ip);
  ip->checksum =
      compute_checksum(bytes, WORDS_TO_BYTES(ip->internet_header_length));
  free(bytes);
}
