#ifndef FT_PING_H_
#define FT_PING_H_
#pragma once

#include <arpa/inet.h>
#include <stdbool.h>

#define IS_COUNT_SET(x) (x & 0x1)
#define IS_INTERVAL_SET(x) (x & (0x1 << 1))
#define IS_TTL_SET(x) (x & (0x1 << 2))
#define IS_TOS_SET(x) (x & (0x1 << 3))
#define IS_VERBOSE_SET(x) (x & (0x1 << 4))
#define IS_TIMEOUT_SET(x) (x & (0x1 << 5))
#define IS_LINGER_SET(x) (x & (0x1 << 6))
#define IS_PRELOAD_SET(x) (x & (0x1 << 7))
#define IS_PATTERN_SET(x) (x & (0x1 << 8))
#define SET_COUNT_FLAG(x) (x | 0x1)
#define SET_INTERVAL_FLAG(x) (x | (0x1 << 1))
#define SET_TTL_FLAG(x) (x | (0x1 << 2))
#define SET_TOS_FLAG(x) (x | (0x1 << 3))
#define SET_VERBOSE_FLAG(x) (x | (0x1 << 4))
#define SET_TIMEOUT_FLAG(x) (x | (0x1 << 5))
#define SET_LINGER_FLAG(x) (x | (0x1 << 6))
#define SET_PRELOAD_FLAG(x) (x | (0x1 << 7))
#define SET_PATTERN_FLAG(x) (x | (0x1 << 8))

typedef uint64_t t_host_time;

/*
** The list of hosts that will be processed with its corresponding packet list
*/
typedef struct s_host {
  char const *host;
  in_addr_t ip;
  uint32_t transmitted;
  uint32_t received;
  t_host_time first_timestamp;
  t_host_time last_timestamp;
  t_host_time total_time_micro;
  t_host_time squared_total_time_micro;
  t_host_time min_time_micro;
  t_host_time max_time_micro;
  struct s_host *next;
} t_host;

/*
** Loaded settings for the ping execution
** All the fields are uint64_t as the socket will validate the correct range
*/
typedef struct s_settings {
  uint16_t flags;
  uint64_t count;
  uint64_t interval;
  uint64_t ttl;
  uint64_t tos;
  bool verbose;
  uint64_t timeout;
  uint64_t linger;
  uint64_t preload;
  char *pattern;
} t_settings;

typedef struct s_ft_ping {
  t_settings settings;
  t_host *hosts;
} t_ft_ping;

void remove_all_hosts(void);
void add_host(char const *host);
void main_loop(void);

#endif // !FT_PING_H_
