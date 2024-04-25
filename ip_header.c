#include "ip_header.h"
#include "utils.h"
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

t_ip_header *create_ip_header(void) {
  t_ip_header *ip;

  ip = malloc(sizeof(t_ip_header));
  if (!ip) {
    printf("An error occurred whilst allocating memory\n");
    exit(1);
  }
  ip->type_of_service = 0;
  ip->fragmentation = 0;
  ip->protocol = 0;
  ip->checksum = 0;
  ip->source = 0;
  ip->destination = 0;
  ip->options = 0;
  ip->version = 0x04;
  ip->internet_header_length = 0x05;
  ip->total_length = 20;
  ip->identification = 0x4242;
  ip->time_to_live = 0xff;
  set_ip_header_checksum(ip);
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
  memcpy(ip, bytes, WORDS_TO_BYTES(packet_ihl_words));
  ip->total_length = ip->total_length;
  ip->identification = ntohs(ip->identification);
  ip->fragmentation = ntohs(ip->fragmentation);
  ip->checksum = ntohs(ip->checksum);
  ip->source = ip->source;
  ip->destination = ip->destination;
  ip->options = ntohl(ip->options);
  return ip;
}

uint8_t *pack_ip_header(t_ip_header const *const ip) {
  t_ip_header copy;
  void *bytes;

  copy = *ip;
  copy.total_length = ip->total_length;
  copy.identification = htons(ip->identification);
  copy.fragmentation = htons(ip->fragmentation);
  copy.checksum = htons(ip->checksum);
  copy.source = ip->source;
  copy.destination = ip->destination;
  copy.options = htonl(ip->options);

  bytes = malloc(sizeof(uint8_t) * WORDS_TO_BYTES(ip->internet_header_length));
  if (!bytes) {
    printf("An error occurred whilst allocating memory\n");
    exit(1);
  }
  memcpy(bytes, &copy, WORDS_TO_BYTES(ip->internet_header_length));
  return bytes;
}

void set_internet_header_length(t_ip_header *const ip) {
  if (ip->options) {
    ip->internet_header_length = 6;
    ip->total_length += WORDS_TO_BYTES(1);
    set_ip_header_checksum(ip);
    return;
  }
  if (ip->internet_header_length != 6)
    return;
  ip->internet_header_length = 5;
  ip->total_length -= WORDS_TO_BYTES(1);
  set_ip_header_checksum(ip);
}

void set_type_of_service(t_ip_header *const ip,
                         t_ip_types_of_service const tos) {
  ip->type_of_service = tos;
  set_ip_header_checksum(ip);
}

void set_body_length(t_ip_header *const ip, uint16_t const length) {
  ip->total_length = WORDS_TO_BYTES(ip->internet_header_length) + length;
  set_ip_header_checksum(ip);
}

void set_identification(t_ip_header *const ip, uint16_t const id) {
  ip->identification = id;
  set_ip_header_checksum(ip);
}

void set_time_to_live(t_ip_header *const ip, uint8_t const ttl) {
  ip->time_to_live = ttl;
  set_ip_header_checksum(ip);
}

void set_protocol(t_ip_header *const ip, t_ip_protocols const protocol) {
  ip->protocol = protocol;
  set_ip_header_checksum(ip);
}

void set_ip_header_checksum(t_ip_header *const ip) {
  void *bytes;

  ip->checksum = 0;
  bytes = pack_ip_header(ip);
  ip->checksum =
      compute_checksum(bytes, WORDS_TO_BYTES(ip->internet_header_length));
  free(bytes);
}

void set_source(t_ip_header *const ip, uint32_t const source) {
  ip->source = source;
  set_ip_header_checksum(ip);
}

void set_destination(t_ip_header *const ip, uint32_t const destination) {
  ip->destination = destination;
  set_ip_header_checksum(ip);
}
