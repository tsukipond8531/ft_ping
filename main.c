#include "icmp.h"
#include "ip_header.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void) {
  t_ip_header *ip_header = create_ip_header();
  t_icmp *icmp = create_icmp(ICMP_ECHO);

  in_addr_t ip = inet_addr("10.5.16.78");
  in_addr_t dest = inet_addr("163.172.250.16");

  set_body_length(ip_header, sizeof(t_icmp));
  set_identification(ip_header, 0x4242);
  set_time_to_live(ip_header, 64);
  set_protocol(ip_header, IP_ICMP);
  set_source(ip_header, ip);
  set_destination(ip_header, dest);

  set_identifier(icmp, 0x1914);

  uint8_t *bytes = icmp_to_packet(ip_header, icmp);
  for (size_t i = 0; i < ip_header->total_length; i++)
    printf("%02x ", *(bytes + i));

  int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
  if (sock < 0)
    exit(42);
  struct sockaddr_in addr;

  addr.sin_port = 42;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = dest;
  int removeheader = 1;
  if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &removeheader,
                 sizeof(removeheader)) < 0)
    exit(44);
  int sent = sendto(sock, bytes, ip_header->total_length, 0,
                    (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
  if (sent < 0) {
    perror("Error whilst sending bytes");
    printf("\nERRNO: %d\n", errno);
    exit(46);
  }
  printf("Bytes sent!\n");

  return 0;
}
