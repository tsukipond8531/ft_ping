#include "icmp.h"
#include "ip_header.h"
#include "utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  memcpy(bytes, &copy, sizeof(t_icmp));
  return bytes;
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
    memcpy(packet->payload.echo.data, data,
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
