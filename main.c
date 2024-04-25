#include "ft_ping.h"
#include "parser.h"
#include "utils.h"
#include <argp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
  t_ft_ping ping;

  bzero(&ping, sizeof(t_ft_ping));
  load_arguments(argc, argv, &ping);
  terminate(0, "Success!", &ping);
  return 0;
}
