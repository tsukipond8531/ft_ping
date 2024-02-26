#include "icmp.h"
#include "ip_header.h"
#include "utils.h"
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void utils_compute_checksum(void) {
  {
    char bytes[20] = {0x45, 0x00, 0x00, 0x73, 0x00, 0x00, 0x40,
                      0x00, 0x40, 0x11, 0x00, 0x00, 0xc0, 0xa8,
                      0x00, 0x01, 0xc0, 0xa8, 0x00, 0xc7};
    uint16_t checksum = compute_checksum(bytes, 20);

    assert(checksum == 0xb861);
  }
  printf("[OK][compute_checksum] Basic\n");
  {
    char bytes[20] = {0x45, 0x00, 0x00, 0x54, 0x20, 0xb8, 0x00,
                      0x00, 0x40, 0x01, 0x00, 0x00, 0xc0, 0xa8,
                      0x00, 0x50, 0xac, 0xd9, 0x10, 0x8e};
    uint16_t checksum = compute_checksum(bytes, 20);

    assert(checksum == 0xdb91);
  }
  printf("[OK][compute_checksum] Basic 2\n");
  {

    char bytes[20] = {0x45, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00,
                      0x00, 0x72, 0x01, 0x00, 0x00, 0xac, 0xd9,
                      0x10, 0x8e, 0xc0, 0xa8, 0x00, 0x50};
    uint16_t checksum = compute_checksum(bytes, 20);

    assert(checksum == 0xca49);
  }
  printf("[OK][compute_checksum] Basic 3\n");
  {

    char bytes[20] = {0x45, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00,
                      0x00, 0x72, 0x02, 0x00, 0x00, 0xac, 0xd9,
                      0x10, 0x8e, 0xc0, 0xa8, 0x00, 0x50};
    uint16_t checksum = compute_checksum(bytes, 20);

    assert(checksum != 0xca49);
  }
  printf("[OK][compute_checksum] Inequality\n");
}

void utils_is_valid_checksum(void) {
  {
    char bytes[20] = {0x45, 0x00, 0x00, 0x73, 0x00, 0x00, 0x40,
                      0x00, 0x40, 0x11, 0xb8, 0x61, 0xc0, 0xa8,
                      0x00, 0x01, 0xc0, 0xa8, 0x00, 0xc7};

    assert(is_valid_checksum(bytes, 20));
  }
  printf("[OK][is_valid_checksum] Basic\n");
  {
    char bytes[20] = {0x45, 0x00, 0x00, 0x54, 0x20, 0xb8, 0x00,
                      0x00, 0x40, 0x01, 0xdb, 0x91, 0xc0, 0xa8,
                      0x00, 0x50, 0xac, 0xd9, 0x10, 0x8e};

    assert(is_valid_checksum(bytes, 20));
  }
  printf("[OK][is_valid_checksum] Basic 2\n");
  {

    char bytes[20] = {0x45, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00,
                      0x00, 0x72, 0x01, 0xca, 0x49, 0xac, 0xd9,
                      0x10, 0x8e, 0xc0, 0xa8, 0x00, 0x50};

    assert(is_valid_checksum(bytes, 20));
  }
  printf("[OK][is_valid_checksum] Basic 3\n");
  {

    char bytes[20] = {0x45, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00,
                      0x00, 0x72, 0x01, 0xcb, 0x49, 0xac, 0xd9,
                      0x10, 0x8e, 0xc0, 0xa8, 0x00, 0x50};

    assert(!is_valid_checksum(bytes, 20));
  }
  printf("[OK][is_valid_checksum] Inequality\n");
}

void utils_ft_memcpy(void) {
  {
    char bytes[16];
    char source[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                       0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    ft_memcpy(bytes, source, 16);
    for (size_t i = 0; i < 16; i++)
      assert(*(bytes + i) == *(source + i));
  }
  printf("[OK][ft_memcpy] Basic byte test\n");
  {
    int bytes[16];
    int source[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                      0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    char *cmpbytes = (char *)bytes;
    char *cmpsource = (char *)source;

    ft_memcpy(bytes, source, 16 * sizeof(int));
    for (size_t i = 0; i < 16 * sizeof(int); i++)
      assert(*(cmpbytes + i) == *(cmpsource + i));
  }
  printf("[OK][ft_memcpy] Different data type size test\n");
}

void ip_header(void) {
  {
    t_ip_header *header = create_ip_header();
    free(header);
  }
  printf("[OK][t_ip_header] Basic create and delete\n");
  {
    t_ip_header *header = create_ip_header();
    uint8_t *bytes;

    set_internet_header_length(header);
    set_type_of_service(header, IP_PREC_CRIT | IP_DELAY_LOW |
                                    IP_THROUGHPUT_HIGH | IP_RELIABILITY_HIGH);
    set_body_length(header, 191);
    set_identification(header, 0x2001);
    set_time_to_live(header, 0x40);
    set_time_to_live(header, 0x40);
    set_protocol(header, IP_ICMP);
    set_source(header, 1294128512);
    set_destination(header, 2412451255);
    bytes = pack_ip_header(header);

    assert(is_valid_checksum(bytes,
                             WORDS_TO_BYTES(header->internet_header_length)));
    free(bytes);
    free(header);
  }
  printf("[OK][t_ip_header] All attribute setter with checksum validation\n");
  {
    t_ip_header *a = create_ip_header();

    set_identification(a, 0x2001);

    uint8_t *b = pack_ip_header(a);
    t_ip_header *c = unpack_ip_header(b);
    uint8_t *d = pack_ip_header(c);
    t_ip_header *e = unpack_ip_header(d);
    uint8_t *f = pack_ip_header(e);
    t_ip_header *g = unpack_ip_header(f);

    assert(g->identification == 0x2001);
    assert(is_valid_checksum(f, WORDS_TO_BYTES(g->internet_header_length)));
    free(g);
    free(f);
    free(e);
    free(d);
    free(c);
    free(b);
    free(a);
  }
  printf(
      "[OK][t_ip_header] Multiple repack to assess checksum and data safety\n");
  {
    t_ip_header *ip_header = create_ip_header();
    in_addr_t ip = inet_addr("10.5.16.78");
    in_addr_t dest = inet_addr("163.172.250.16");

    set_body_length(ip_header, sizeof(t_icmp));
    set_identification(ip_header, 0x4242);
    set_time_to_live(ip_header, 64);
    set_protocol(ip_header, IP_ICMP);
    set_source(ip_header, ip);
    set_destination(ip_header, dest);

    uint8_t *bytes = pack_ip_header(ip_header);

    t_ip_header *ip_header2 = unpack_ip_header(bytes);

    assert(ip_header->total_length == ip_header2->total_length);
    assert(ip_header->source == ip_header2->source);
    assert(ip_header->destination == ip_header2->destination);
  }
  printf("[OK][t_ip_header] Check of source and destination pack / unpack "
         "matching\n");
}

void icmp(void) {
  uint8_t empty_data[32] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
  uint8_t random_data[32] = {0x42, 0x42, 0x42, 0x42, 0x42,
                             0x42, 0x42, 0x42, 0x42};

  {
    t_icmp *icmp = create_icmp(ICMP_ECHO);

    assert(icmp->type == ICMP_ECHO);
    assert(icmp->code == 0);
    assert(icmp->payload.echo.sequence == 0);
    assert(icmp->payload.echo.identifier == 0x4242);
    assert(!memcmp(icmp->payload.echo.data, empty_data, 32));

    free(icmp);
  }
  printf("[OK][t_icmp] Basic ICMP creation\n");
  // Set basic sequence and identifier
  {
    t_icmp *icmp = create_icmp(ICMP_ECHO);

    set_sequence(icmp, 19);
    assert(icmp->payload.echo.sequence == 19);

    set_sequence_more(icmp);
    assert(icmp->payload.echo.sequence == 20);

    set_identifier(icmp, 0x4242);
    assert(icmp->payload.echo.identifier == 0x4242);
    free(icmp);
  }
  printf("[OK][t_icmp] Set basic seq and identifier\n");
  {
    t_icmp *icmp = create_icmp(ICMP_ECHO);

    set_identifier(icmp, 0x7c8d);
    set_sequence(icmp, 0x1c00);
    set_data(icmp, random_data, 9);
    assert(icmp->checksum == 0x1469);

    free(icmp);
  }
  printf("[OK][t_icmp] Checksum test\n");
  {
    t_icmp *icmp = create_icmp(219);
    assert(errno == EINVAL);
    assert(icmp == NULL);
  }
  printf("[OK][t_icmp] Create non-registered ICMP kind\n");
  {
    t_icmp *icmp = create_icmp(ICMP_ECHO);
    uint8_t *bytes;
    uint8_t data[32] = {0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};

    set_identifier(icmp, 0x7ceb);
    set_sequence(icmp, 0x0400);
    set_data(icmp, data, 32);

    bytes = pack_icmp_payload(icmp);
    assert(is_valid_checksum(bytes, sizeof(t_icmp)));

    free(bytes);
    free(icmp);
  }
  printf("[OK][t_icmp] Checksum validation after packing\n");
  {
    t_ip_header *header = create_ip_header();
    t_icmp *icmp = create_icmp(ICMP_ECHO);

    set_body_length(header, sizeof(t_icmp));
    set_protocol(header, IP_ICMP);
    set_time_to_live(header, 64);

    set_identifier(icmp, 0x1942);
    set_sequence(icmp, 0x199);

    uint8_t *p1 = icmp_to_packet(header, icmp);
    assert(p1 != NULL);
    t_ip_header *hu1 = unpack_icmp_header_from_packet(p1);
    t_icmp *u1 = unpack_icmp_payload_from_packet(p1);
    uint8_t *p2 = icmp_to_packet(hu1, u1);
    assert(p2 != NULL);
    t_ip_header *hu2 = unpack_icmp_header_from_packet(p2);
    t_icmp *u2 = unpack_icmp_payload_from_packet(p2);
    uint8_t *p3 = icmp_to_packet(hu2, u2);
    assert(p3 != NULL);
    t_ip_header *hu3 = unpack_icmp_header_from_packet(p3);
    t_icmp *u3 = unpack_icmp_payload_from_packet(p3);

    assert(hu3->protocol == IP_ICMP);
    assert(hu3->total_length ==
           WORDS_TO_BYTES(hu3->internet_header_length) + sizeof(t_icmp));
    assert(hu3->time_to_live == 64);
    assert(u3->payload.echo.identifier == 0x1942);
    assert(u3->payload.echo.sequence == 0x199);
    free(hu3);
    free(u3);
    free(p3);
    free(hu2);
    free(u2);
    free(p2);
    free(hu1);
    free(u1);
    free(p1);
    free(icmp);
    free(header);
  }
  printf("[OK][t_icmp] Created - pack - unpack - pack - unpack test IP header "
         "and ICMP payload\n");
  {
    t_ip_header *ip_header = create_ip_header();
    t_icmp *icmp = create_icmp(ICMP_ECHO);
    uint8_t *bytes;

    set_body_length(ip_header, sizeof(t_icmp));
    set_identification(ip_header, 0x4242);
    set_time_to_live(ip_header, 64);
    set_protocol(ip_header, IP_ICMP);
    set_source(ip_header, 0xc0a80050);
    set_destination(ip_header, 0x08080808);

    set_identifier(icmp, 0x4141);

    bytes = icmp_to_packet(ip_header, icmp);
    assert(bytes != NULL);
    free(bytes);
    free(icmp);
    free(ip_header);
  }
  printf("[OK][t_icmp] Basic normal usage\n");
}

int main(void) {
  utils_compute_checksum();
  utils_is_valid_checksum();
  utils_ft_memcpy();
  ip_header();
  icmp();
}
