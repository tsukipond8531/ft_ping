#include "packet.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t *icmp_to_packet(t_ip_header const *const header,
                        t_icmp const *const payload) {
  uint8_t *bytes;
  uint8_t *header_bytes;
  uint8_t *payload_bytes;

  if (header->type_of_service != 0)
    return NULL;
  if (header->total_length !=
      WORDS_TO_BYTES(header->internet_header_length) + sizeof(t_icmp))
    return NULL;
  if (header->protocol != 1)
    return NULL;
  header_bytes = pack_ip_header(header);
  payload_bytes = pack_icmp_payload(payload);
  bytes = malloc(header->total_length);
  if (!bytes) {
    printf("An error occurred whilst allocating memory\n");
    exit(1);
  }
  memcpy(bytes, header_bytes, WORDS_TO_BYTES(header->internet_header_length));
  memcpy(bytes + WORDS_TO_BYTES(header->internet_header_length), payload_bytes,
         sizeof(*payload));
  free(payload_bytes);
  free(header_bytes);
  return bytes;
}

t_ip_header *unpack_icmp_header_from_packet(void const *const bytes) {
  return unpack_ip_header(bytes);
}

t_icmp *unpack_icmp_payload_from_packet(void const *const bytes) {
  t_ip_header *ip;
  t_icmp *icmp;

  ip = unpack_icmp_header_from_packet(bytes);
  if (!is_valid_checksum(bytes + WORDS_TO_BYTES(ip->internet_header_length),
                         ip->total_length -
                             WORDS_TO_BYTES(ip->internet_header_length))) {
    free(ip);
    return NULL;
  }
  icmp = malloc(sizeof(t_icmp));
  if (!icmp) {
    printf("An error occurred whilst allocating memory\n");
    exit(1);
  }
  memcpy(icmp, bytes + WORDS_TO_BYTES(ip->internet_header_length),
         ip->total_length - WORDS_TO_BYTES(ip->internet_header_length));
  icmp->checksum = ntohs(icmp->checksum);
  if (icmp->type == ICMP_ECHO || icmp->type == ICMP_ECHO_REPLY) {
    icmp->payload.echo.identifier = ntohs(icmp->payload.echo.identifier);
    icmp->payload.echo.sequence = ntohs(icmp->payload.echo.sequence);
  }
  free(ip);
  return icmp;
}
