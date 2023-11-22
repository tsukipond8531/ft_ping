#include "ip_header.h"
#include "utils.h"
#include <assert.h>
#include <stdlib.h>

void utils_compute_checksum(void) {
  {
    char bytes[20] = {0x45, 0x00, 0x00, 0x73, 0x00, 0x00, 0x40,
                      0x00, 0x40, 0x11, 0x00, 0x00, 0xc0, 0xa8,
                      0x00, 0x01, 0xc0, 0xa8, 0x00, 0xc7};
    uint16_t checksum = compute_checksum(bytes, 20);

    assert(checksum == 0xb861);
  }
  {
    char bytes[20] = {0x45, 0x00, 0x00, 0x54, 0x20, 0xb8, 0x00,
                      0x00, 0x40, 0x01, 0x00, 0x00, 0xc0, 0xa8,
                      0x00, 0x50, 0xac, 0xd9, 0x10, 0x8e};
    uint16_t checksum = compute_checksum(bytes, 20);

    assert(checksum == 0xdb91);
  }
  {

    char bytes[20] = {0x45, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00,
                      0x00, 0x72, 0x01, 0x00, 0x00, 0xac, 0xd9,
                      0x10, 0x8e, 0xc0, 0xa8, 0x00, 0x50};
    uint16_t checksum = compute_checksum(bytes, 20);

    assert(checksum == 0xca49);
  }
  {

    char bytes[20] = {0x45, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00,
                      0x00, 0x72, 0x02, 0x00, 0x00, 0xac, 0xd9,
                      0x10, 0x8e, 0xc0, 0xa8, 0x00, 0x50};
    uint16_t checksum = compute_checksum(bytes, 20);

    assert(checksum != 0xca49);
  }
}

void utils_is_valid_checksum(void) {
  {
    char bytes[20] = {0x45, 0x00, 0x00, 0x73, 0x00, 0x00, 0x40,
                      0x00, 0x40, 0x11, 0xb8, 0x61, 0xc0, 0xa8,
                      0x00, 0x01, 0xc0, 0xa8, 0x00, 0xc7};

    assert(is_valid_checksum(bytes, 20));
  }
  {
    char bytes[20] = {0x45, 0x00, 0x00, 0x54, 0x20, 0xb8, 0x00,
                      0x00, 0x40, 0x01, 0xdb, 0x91, 0xc0, 0xa8,
                      0x00, 0x50, 0xac, 0xd9, 0x10, 0x8e};

    assert(is_valid_checksum(bytes, 20));
  }
  {

    char bytes[20] = {0x45, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00,
                      0x00, 0x72, 0x01, 0xca, 0x49, 0xac, 0xd9,
                      0x10, 0x8e, 0xc0, 0xa8, 0x00, 0x50};

    assert(is_valid_checksum(bytes, 20));
  }
  {

    char bytes[20] = {0x45, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00,
                      0x00, 0x72, 0x01, 0xcb, 0x49, 0xac, 0xd9,
                      0x10, 0x8e, 0xc0, 0xa8, 0x00, 0x50};

    assert(!is_valid_checksum(bytes, 20));
  }
}

void utils_ft_memcpy(void) {
  // Basic byte test
  {
    char bytes[16];
    char source[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                       0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    ft_memcpy(bytes, source, 16);
    for (size_t i = 0; i < 16; i++)
      assert(*(bytes + i) == *(source + i));
  }
  // Different data type test
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
}

void ip_header(void) {
  // Basic create and delete
  {
    t_ip_header *header = create_ip_header();
    free(header);
  }
  // All attribute setter with checksum validation
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
  // Multiple repack to assess checksum and data safety
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
}

int main(void) {
  utils_compute_checksum();
  utils_is_valid_checksum();
  utils_ft_memcpy();
  ip_header();
}
