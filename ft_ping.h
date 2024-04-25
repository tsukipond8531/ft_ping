#ifndef FT_PING_H_
#define FT_PING_H_
#pragma once

#include "icmp.h"
#include "ip_header.h"
#include <arpa/inet.h>
#include <stdbool.h>

/*
** An ICMP ECHO_REQUEST linked list of all packets sent
*/
typedef struct s_packet {
  uint64_t timestamp;
  t_ip_header *header;
  t_icmp *icmp;
  struct s_packet *next;
} t_packet;

/*
** The list of hosts that will be processed with its corresponding packet list
*/
typedef struct s_host {
  char const *host;
  struct in_addr ip;
  uint32_t transmitted;
  uint32_t received;
  uint32_t duplicated;
  t_packet *packets;
  struct s_host *next;
} t_host;

/*
** Loaded settings for the ping execution
*/
typedef struct s_settings {
  uint32_t count;
  uint32_t interval;
  uint8_t ttl;
  uint8_t tos;
  bool verbose;
  uint32_t timeout;
  uint32_t linger;
  uint16_t preload;
  uint8_t pattern[DATA_SIZE];
} t_settings;

typedef struct s_ft_ping {
  t_settings settings;
  t_host *hosts;
} t_ft_ping;

void remove_all_hosts(t_ft_ping *ping);
void add_host(t_ft_ping *ping, char const *host);
int resolve_host(t_host *host);
void add_packet(t_ft_ping *ping, t_host *host, t_ip_header *header,
                t_icmp *icmp);
t_packet *get_packet_by_seq(t_host const *host, uint16_t const sequence);
void main_loop(t_ft_ping *ping);

#endif // !FT_PING_H_
