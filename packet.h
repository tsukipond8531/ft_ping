#ifndef PACKET_H_
#define PACKET_H_
#include "icmp.h"
#include "ip_header.h"
#pragma once

/*
** icmp_to_packet
**
** Allocates a fixed and packed version of the bytes between
** the IP header and the ICMP payload, making sure to set and
** verify the checksum if needed.
**
** All pertinent validations (such as the type ICMP in the
** IP header) will be checked before formatting the packet.
** If an invalid or incongruous value is found, the packet is
** discarded without the arguments being free and NULL is set.
**
** The result is a byte sequence that can be sent through a raw
** socket and will be understood as an ICMP packet
*/
uint8_t *icmp_to_packet(t_ip_header const *const header,
                        t_icmp const *const payload);

/*
** unpack_icmp_header_from_packet
**
** The input is expected to be a fully fledged packet, and the
** return value will be the deconstructed packet's header.
** If the checksum were to be invalid, or any other error or
** incongruence was found in the process, NULL would be
** returned instead.
**
** Wrapper of unpack_ip_header which will dynamically take
** strictly the necessary bytes
*/
t_ip_header *unpack_icmp_header_from_packet(void const *const bytes);

/*
** unpack_icmp_payload_from_packet
**
** The input is expected to be a fully fledged packet, and the
** return value will be the deconstructed packet's payload.
** If the checksum were to be invalid, or any other error or
** incongruence was found in the process, NULL would be
** returned instead
*/
t_icmp *unpack_icmp_payload_from_packet(void const *const bytes);

#endif // !PACKET_H_
