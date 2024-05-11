#include "icmp.h"
#include "utils.h"
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

uint8_t *icmp_bytes(t_icmp icmp, uint8_t const *data, uint16_t *datalen) {
  uint8_t *bytes;
  uint16_t total_size;

  total_size = sizeof(icmp);
  if (datalen)
    total_size += *datalen;
  if (!(bytes = malloc(total_size)))
    terminate(1, "ft_ping: malloc failed");

  icmp.checksum = 0;
  icmp.sequence = htons(icmp.sequence);
  icmp.identifier = htons(icmp.identifier);
  icmp.time.tv_sec = htonl(icmp.time.tv_sec);
  icmp.time.tv_usec = htonl(icmp.time.tv_usec);

  memcpy(bytes, &icmp, sizeof(icmp));
  if (data && datalen)
    memcpy(bytes + sizeof(icmp), data, *datalen);
  if (datalen)
    *datalen = total_size;

  icmp.checksum = compute_checksum(bytes, total_size);
  memcpy(bytes, &icmp, sizeof(icmp));

  return bytes;
}

int icmp_from_bytes(t_icmp *icmp, uint8_t const *const bytes, uint16_t len) {
  if (!is_valid_checksum(bytes, len))
    return 1;
  // Shallow copying is the most efficient way of implementing it
  *icmp = *(t_icmp *)bytes;
  icmp->sequence = ntohs(icmp->sequence);
  icmp->identifier = ntohs(icmp->identifier);
  icmp->time.tv_sec = ntohl(icmp->time.tv_sec);
  icmp->time.tv_usec = ntohl(icmp->time.tv_usec);
  return 0;
}
