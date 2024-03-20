#ifndef FT_PING_H_
#define FT_PING_H_
#pragma once

#include "icmp.h"
#include "ip_header.h"

typedef struct s_ft_ping {
  t_ip_header *header;
  t_icmp *icmp;
} t_ft_ping;

#endif // !FT_PING_H_
