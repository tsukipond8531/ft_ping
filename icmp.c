#include "icmp.h"
#include "ip_header.h"
#include "utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

t_icmp *create_icmp(t_icmp_types const type) {
  t_icmp *icmp;

  icmp = malloc(sizeof(t_icmp));
  if (!icmp) {
    printf("An error occurred whilst allocating memory\n");
    exit(1);
  }
  icmp->code = 0;
  icmp->checksum = 0;
  icmp->payload.echo.identifier = 0;
  icmp->payload.echo.sequence = 0;
  for (size_t i = 0; i < DATA_SIZE; i++)
    icmp->payload.echo.data[i] = 0;
  icmp->type = type;
  switch (icmp->type) {
  case ICMP_ECHO:
  case ICMP_ECHO_REPLY:
    // This behaviour is undefined for ICMP_ECHO_REPLY as the ICMP_ECHO_REPLY
    // should be provided of an identifier/seq matching the original ICMP_ECHO
    icmp->payload.echo.identifier = 0x4242;
    break;
  default:
    errno = EINVAL;
    free(icmp);
    return NULL;
  }
  set_icmp_checksum(icmp);
  return icmp;
}

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
  ft_memcpy(bytes, header_bytes,
            WORDS_TO_BYTES(header->internet_header_length));
  ft_memcpy(bytes + WORDS_TO_BYTES(header->internet_header_length),
            payload_bytes, sizeof(*payload));
  free(payload_bytes);
  free(header_bytes);
  return bytes;
}

uint8_t *pack_icmp_payload(t_icmp const *const payload) {
  t_icmp copy;
  void *bytes;

  copy = *payload;
  copy.checksum = htons(payload->checksum);
  if (payload->type == ICMP_ECHO || payload->type == ICMP_ECHO_REPLY) {
    copy.payload.echo.identifier = htons(payload->payload.echo.identifier);
    copy.payload.echo.sequence = htons(payload->payload.echo.sequence);
  }

  bytes = malloc(sizeof(t_icmp));
  if (!bytes) {
    printf("An error occurred whilst allocating memory\n");
    exit(1);
  }
  ft_memcpy(bytes, &copy, sizeof(t_icmp));
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
  ft_memcpy(icmp, bytes + WORDS_TO_BYTES(ip->internet_header_length),
            ip->total_length - WORDS_TO_BYTES(ip->internet_header_length));
  icmp->checksum = ntohs(icmp->checksum);
  if (icmp->type == ICMP_ECHO || icmp->type == ICMP_ECHO_REPLY) {
    icmp->payload.echo.identifier = ntohs(icmp->payload.echo.identifier);
    icmp->payload.echo.sequence = ntohs(icmp->payload.echo.sequence);
  }
  free(ip);
  return icmp;
}

void set_identifier(t_icmp *const packet, uint16_t identifier) {
  if (packet->type == ICMP_ECHO || packet->type == ICMP_ECHO_REPLY)
    packet->payload.echo.identifier = identifier;
  set_icmp_checksum(packet);
}

void set_sequence(t_icmp *const packet, uint16_t const seq) {
  if (packet->type == ICMP_ECHO || packet->type == ICMP_ECHO_REPLY)
    packet->payload.echo.sequence = seq;
  set_icmp_checksum(packet);
}

void set_sequence_more(t_icmp *const packet) {
  if (packet->type == ICMP_ECHO || packet->type == ICMP_ECHO_REPLY)
    packet->payload.echo.sequence++;
  set_icmp_checksum(packet);
}

void set_data(t_icmp *const packet, uint8_t data[DATA_SIZE],
              uint8_t data_size) {
  if (packet->type == ICMP_ECHO || packet->type == ICMP_ECHO_REPLY)
    ft_memcpy(packet->payload.echo.data, data,
              data_size > DATA_SIZE ? DATA_SIZE : data_size);
  set_icmp_checksum(packet);
}

void set_icmp_checksum(t_icmp *const packet) {
  void *bytes;

  packet->checksum = 0;
  bytes = pack_icmp_payload(packet);
  packet->checksum = compute_checksum(bytes, sizeof(t_icmp));
  free(bytes);
}
