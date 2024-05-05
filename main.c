#include "ft_ping.h"
#include "parser.h"
#include "utils.h"

t_ft_ping ping;

int main(int argc, char **argv) {
  load_arguments(argc, argv);
  host_loop(ping.hosts);
  terminate(0, "Success!");
  return 0;
}
