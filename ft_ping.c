#include "ft_ping.h"
#include "icmp.h"
#include "utils.h"
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  double average_micro;
  double variance;
  double stddev;

  printf("--- %s ping statistics ---\n", host->host);
  printf("%u packets transmitted, ", host->transmitted);
  printf("%u packets received, ", host->received);
  packet_loss = 0;
  if (host->transmitted > 0)
    packet_loss = 100 - (host->received / host->transmitted * 100);
  printf("%u%% packet loss\n", packet_loss);
  printf("round-trip min/avg/max/stddev = ");
  printf("%.3f/", host->min_time_micro / 1000.0);
  average_micro = 0.0;
  variance = 0.0;
  stddev = 0.0;
  if (host->transmitted > 0) {
    average_micro = (double)host->total_time_micro / host->transmitted;
    variance = (double)host->squared_total_time_micro / host->transmitted -
               average_micro * average_micro;
    stddev = sqrt(variance);
  }
  printf("%.3f/", average_micro / 1000.0);
  printf("%.3f/", host->max_time_micro / 1000.0);
  printf("%.3f ms\n", stddev / 1000.0);
}

static inline void print_host_header(t_host const *const host) {
  char *ip_text;
  struct in_addr addr;

  addr.s_addr = host->ip;
  ip_text = inet_ntoa(addr);
  printf("PING %s (%s): %u data bytes", host->host, ip_text, DATA_SIZE);
  if (IS_VERBOSE_SET(ping.settings.flags))
    printf(", id %#04x = %hu", icmp_get_id(), icmp_get_id());
  puts("");
}

static inline void print_packet(uint8_t const *const packet) {
  struct iphdr const *const ip = (struct iphdr *)packet;
  t_icmp const *const icmp = (t_icmp *)(packet + 20);

  printf("IP Hdr Dump:\n ");
  for (size_t i = 0; i < WORDS_TO_BYTES(ip->ihl); i++)
    printf("%02x%s", *((uint8_t *)ip + i), i % 2 ? " " : "");
  puts("\nVr HL TOS  Len   ID Flg  off TTL Pro  cks      Src	Dst");
  printf(" %hhu  %hhu %03hhu %04hu %02x   %hu %04hu  "
         "%03hhu  %02hhu %04hx %s %s\n",
         ip->version, ip->ihl, ip->tos, ntohs(ip->tot_len), ntohs(ip->id),
         ntohs(ip->frag_off) >> 13, ntohs(ip->frag_off) & 0x1fff, ip->ttl,
         ip->protocol, ntohs(ip->check),
         inet_ntoa(*(struct in_addr *)&ip->saddr),
         inet_ntoa(*(struct in_addr *)&ip->daddr));
  printf("ICMP: type %hhu, code %hhu, size %hu", icmp->type, icmp->code,
         ntohs(ip->tot_len) - WORDS_TO_BYTES(ip->ihl));
  if (icmp->type == ICMP_ECHO || icmp->type == ICMP_ECHO_REPLY)
    printf(", id %#04x, seq %#04x", ntohs(icmp->identifier),
           ntohs(icmp->sequence));
  puts("");
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
  host->ip = host_as_ip;
  return 0;
}

static int host_setup_socket(void) {
  int sockfd;
  uint64_t sockopt;
  struct linger lingeropts;

  if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
    terminate(1, "ft_ping: unexpected error whilst creating socket");
  if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)
    terminate(1, "ft_ping: unexpected error whilst configuring FD");
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

static inline bool should_loop(t_host const *const host) {
  // If timeout is set, check if it has expired
  if (IS_TIMEOUT_SET(ping.settings.flags)) {
    bool const timeout_expired = get_time_micro() - host->first_timestamp >
                                 ping.settings.timeout * 1000000;
    if (timeout_expired)
      return false;
  }

  // If count is set, check if it has been reached
  if (IS_COUNT_SET(ping.settings.flags)) {
    bool const count_reached = host->received >= ping.settings.count;
    if (count_reached)
      return false;
  }

  return true;
}

static inline bool should_send_packet(t_host const *const host) {
  t_host_time const now = get_time_micro();
  uint64_t interval;

  // The first packet should be instantly emitted
  if (host->last_timestamp == 0)
    return true;

  // The default interval is 1 second, but can be overwritten by the settings
  interval = 1;
  if (IS_INTERVAL_SET(ping.settings.flags))
    interval = ping.settings.interval;

  if (now - host->last_timestamp <= interval * 1000000)
    return false;
  return true;
}

static inline void send_packet(int const sockfd, t_host const *const host) {
  t_icmp icmp = {0};
  struct sockaddr_in addr;
  struct timeval now;
  uint8_t *icmp_payload;
  uint16_t icmp_len;
  uint8_t icmp_data[DATA_SIZE + 1];

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = host->ip;

  icmp.type = ICMP_ECHO;
  icmp.identifier = icmp_get_id();
  icmp.sequence = host->transmitted;

  bzero(icmp_data, DATA_SIZE + 1);
  if (IS_PATTERN_SET(ping.settings.flags)) {
    strncpy((char *)icmp_data, ping.settings.pattern, DATA_SIZE);
  }

  gettimeofday(&now, NULL);
  icmp.time = now;

  icmp_len = DATA_SIZE + 1;
  icmp_payload = icmp_bytes(icmp, icmp_data, &icmp_len);
  if (sendto(sockfd, icmp_payload, icmp_len, 0, (struct sockaddr *)&addr,
             (socklen_t)sizeof(addr)) < 0) {
    perror("ft_ping: error whilst sending packet (continuing...)");
  }
  free(icmp_payload);
}

static inline void receive_packet(int const sockfd, t_host *const host) {
  struct sockaddr_in addr;
  uint8_t buffer[1024];
  ssize_t length;
  t_icmp icmp;

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = host->ip;
  length = sizeof(addr);

  if ((length = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr,
                         (socklen_t *)&length)) < 0) {
    if (errno != EAGAIN && errno != EWOULDBLOCK)
      perror("ft_ping: error whilst receiving packet (continuing...)");
    return;
  }

  if (icmp_from_bytes(&icmp, buffer) != 0)
    return;

  if (IS_VERBOSE_SET(ping.settings.flags) && icmp.type != ICMP_ECHO_REPLY) {
    print_packet(buffer);
    host->received++;
    return;
  }

  if (icmp.identifier != getpid())
    return;

  char *ip_txt = inet_ntoa(addr.sin_addr);

  printf("64 bytes from %s: icmp_seq=%u ttl=%u time=28.199ms\n", ip_txt,
         icmp.sequence, buffer[8]);
  host->received++;
}

static void host_loop(int const sockfd, t_host *const host) {
  t_host_time time;

  host->first_timestamp = get_time_micro();
  while (should_loop(host)) {
    time = get_time_micro();
    if (should_send_packet(host)) {
      send_packet(sockfd, host);
      host->transmitted++;
      host->last_timestamp = time;
    }
    receive_packet(sockfd, host);
    usleep(20);
  }
}

void main_loop(void) {
  int const sockfd = host_setup_socket();

  for (t_host *host = ping.hosts; host; host = host->next) {
    if (resolve_host(host) != 0)
      terminate(1, "ft_ping: host has invalid name");
    print_host_header(host);
    host_loop(sockfd, host);
    print_host_stats(host);
  }
  close(sockfd);
}
