#include "ft_ping.h"
#include "utils.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

extern t_ft_ping ping;

void remove_all_hosts() {
  t_host *htmp;

  htmp = ping.hosts;
  for (t_host *host = htmp; host; host = htmp) {
    htmp = host->next;
    free(host);
  }
}

void add_host(char const *host) {
  t_host **head;
  t_host *newhost;

  newhost = calloc(sizeof(t_host), 1);
  if (!newhost)
    terminate(1, "Allocation error");
  newhost->host = host;
  head = &(ping.hosts);
  while (*head)
    *head = (*head)->next;
  *head = newhost;
}

static int resolve_host(t_host *host) {
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

static int host_setup_socket(t_host *host) {
  int sockfd;
  uint64_t sockopt;
  struct linger lingeropts;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP)) < 0)
    terminate(1, "ft_ping: unexpected error whilst creating socket");
  if (IS_TTL_SET(ping.settings.flags)) {
    sockopt = ping.settings.ttl;
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &sockopt, sizeof(uint64_t)) < 0)
      terminate(1, "ft_ping: invalid TTL (0 <= TTL <= 255)");
  }
  if (IS_TOS_SET(ping.settings.flags)) {
    sockopt = ping.settings.tos;
    if (setsockopt(sockfd, IPPROTO_IP, IP_TOS, &sockopt, sizeof(uint64_t)) < 0)
      terminate(1, "ft_ping: invalid type of service (see <netinet/in.h>)");
  }
  if (IS_LINGER_SET(ping.settings.flags)) {
    lingeropts.l_onoff = 1;
    lingeropts.l_linger = ping.settings.linger;
    if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &lingeropts,
                   sizeof(struct linger)) < 0)
      terminate(1, "ft_ping: invalid linger");
  }

  return sockfd;
}

int host_loop(t_host *host) {
  int sockfd;
  fd_set *read;
  fd_set *write;

  if (resolve_host(host) != 0)
    terminate(1, "ft_ping: host has invalid name");
  sockfd = host_setup_socket(host);
  return 0;
}
