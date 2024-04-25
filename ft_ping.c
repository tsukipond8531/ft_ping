#include "ft_ping.h"
#include "utils.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void remove_all_hosts(t_ft_ping *ping) {
  t_host *htmp;
  t_packet *ptmp;

  htmp = ping->hosts;
  for (t_host *host = htmp; host; host = htmp) {
    htmp = host->next;
    ptmp = host->packets;
    for (t_packet *packet = ptmp; packet; packet = ptmp) {
      ptmp = packet->next;
      free(packet->header);
      free(packet->icmp);
      free(packet);
    }
    free(host);
  }
}

void add_host(t_ft_ping *ping, char const *host) {
  t_host **head;
  t_host *newhost;

  newhost = calloc(sizeof(t_host), 1);
  if (!newhost)
    terminate(1, "Allocation error", ping);
  newhost->host = host;
  head = &(ping->hosts);
  while (*head)
    *head = (*head)->next;
  *head = newhost;
}

int resolve_host(t_host *host) {
  struct addrinfo *host_as_host;
  in_addr_t host_as_ip;

  host_as_ip = inet_addr(host->host);
  if (host_as_ip == INADDR_NONE) {
    if (getaddrinfo(host->host, NULL, NULL, &host_as_host) != 0)
      return 1;
    for (struct addrinfo *curr = host_as_host;
         curr && host_as_ip == INADDR_NONE; curr = curr->ai_next) {
      struct sockaddr_in *addr = (struct sockaddr_in *)curr->ai_addr;
      if (curr->ai_family == AF_INET)
        host_as_ip = addr->sin_addr.s_addr;
    }
    freeaddrinfo(host_as_host);
    if (host_as_ip == INADDR_NONE) {
      return 1;
    }
  }
  host->ip.s_addr = host_as_ip;
  return 0;
}

void add_packet(t_ft_ping *ping, t_host *host, t_ip_header *header,
                t_icmp *icmp) {
  t_packet **head;
  t_packet *new;

  new = calloc(sizeof(t_packet), 1);
  if (!new)
    terminate(1, "Allocation error", ping);

  new->header = header;
  new->icmp = icmp;

  head = &(host->packets);
  while (*head)
    *head = (*head)->next;
  *head = new;
}

t_packet *get_packet_by_seq(t_host const *host, uint16_t const sequence) {
  for (t_packet *current = host->packets; current; current = current->next) {
    if (current->icmp->payload.echo.sequence == sequence)
      return current;
  }
  return NULL;
}

void main_loop(t_ft_ping *ping) {}
