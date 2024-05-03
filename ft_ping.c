#include "icmp.h"
#include "utils.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void remove_all_hosts(t_ft_ping *ping) {
  t_host *htmp;
  t_packet *ptmp;

  htmp = ping->hosts;
  for (t_host *host = htmp; host; host = htmp) {
    htmp = host->next;
    ptmp = host->packets;
    for (t_packet *packet = ptmp; packet; packet = ptmp) {
      ptmp = packet->next;
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

int host_loop(t_settings const *const settings, t_host *host) {
  int sockfd;
  fd_set *read;
  fd_set *write;

  if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
    return 1;

  return 0;
}
