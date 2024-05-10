#include "ft_ping.h"
#include "utils.h"
#include <math.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

extern t_ft_ping ping;

void remove_all_hosts(void) {
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
    head = &((*head)->next);
  *head = newhost;
}

void print_host_stats(t_host const *const host) {
  uint8_t packet_loss;
  double average;
  double variance;
  double stddev;

  printf("--- %s ping statistics ---\n", host->host);
  printf("%u packets transmitted, ", host->transmitted);
  printf("%u packets received, ", host->received);
  if (host->duplicated > 0)
    printf("%+d duplicates, ", host->duplicated);
  packet_loss = 0;
  if (host->transmitted > 0)
    packet_loss = 100 - (host->received / host->transmitted * 100);
  printf("%u%% packet loss\n", packet_loss);
  printf("round-trip min/avg/max/stddev = ");
  printf("%.3f/", host->min_time / 1000.0);
  average = 0.0;
  variance = 0.0;
  stddev = 0.0;
  if (host->transmitted > 0) {
    average = host->total_time / (host->transmitted * 1000.0);
    variance = (double)host->squared_total_time / (double)host->transmitted -
               average * average;
    stddev = sqrt(variance);
  }
  printf("%.3f/", average);
  printf("%.3f/", host->max_time / 1000.f);
  printf("%.3f ms\n", stddev);
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

static int host_setup_socket(void) {
  int sockfd;
  uint64_t sockopt;
  struct linger lingeropts;

  if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
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

static inline bool should_loop(t_host *host) {
  // If timeout is set, check if it has expired
  if (IS_TIMEOUT_SET(ping.settings.flags)) {
    bool const timeout_expired =
        get_time_micro() - host->first_time > ping.settings.timeout * 1000000;
    if (timeout_expired)
      return false;
  }

  // If count is set, check if it has been reached
  if (IS_COUNT_SET(ping.settings.flags)) {
    bool const count_reached = host->transmitted >= ping.settings.count;
    if (count_reached)
      return false;
  }

  return true;
}

static inline bool should_send_packet(t_host *host) {
  t_host_time const now = get_time_micro();
  uint64_t interval;

  // The default interval is 1 second, but can be overwritten by the settings
  interval = 1;
  if (IS_INTERVAL_SET(ping.settings.flags))
    interval = ping.settings.interval;

  if (now - host->last_time <= interval * 1000000)
    return false;
  return true;
}

static void host_loop(int const sockfd, t_host *host) {
  t_host_time time;

  if (resolve_host(host) != 0)
    terminate(1, "ft_ping: host has invalid name");

  (void)sockfd; // TODO Remove
  time = get_time_micro();
  host->first_time = time;
  host->last_time = time;
  while (should_loop(host)) {
    if (should_send_packet(host)) {
      printf("64 bytes from %u\n", host->ip.s_addr); // TODO Actual ping
      host->transmitted++;
      host->total_time += host->last_time - time;
      host->squared_total_time +=
          (host->last_time - time) * (host->last_time - time);
      host->last_time = time;
    }
    usleep(20);
    time = get_time_micro();
  }
}

void main_loop(void) {
  int const sockfd = host_setup_socket();

  for (t_host *host = ping.hosts; host; host = host->next) {
    host_loop(sockfd, host);
    print_host_stats(host);
  }
  close(sockfd);
}
