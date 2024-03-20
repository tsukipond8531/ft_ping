#include "ft_ping.h"
#include <argp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
  struct s_ft_ping ping = {0, 0};
  int iarghost;
  struct argp args = {0, 0, 0, 0, 0, 0, 0};
  struct argp_option options[] = {
      {"count", 'c', "NUMBER", 0, "stop after sending NUMBER packets", 0},
      {"interval", 'i', "NUMBER", 0,
       "wait NUMBER seconds between sending each packet", 0},
      {"ttl", 0x04, "N", 0, "specify N as time-to-live", 0},
      {"tos", 'T', "NUM", 0, "set type of service (TOS) to NUM", 0},
      {"verbose", 'v', 0, 0, "verbose output", 0},
      {"timeout", 'w', "N", 0, "stop after N seconds", 0},
      {"linger", 'W', "N", 0, "number of seconds to wait for response", 0},
      {"flood", 'f', 0, 0, "flood ping", 0},
      {"preload", 'l', "NUMBER", 0,
       "send NUMBER packets as fast as possible before\nfalling into normal "
       "mode of behavior",
       0},
      {"pattern", 'p', "PATTERN", 0,
       "fill ICMP packet with given pattern (hex)", 0},
      {0}};

  argp_program_bug_address = "<mmartin-@student.42berlin.de>";
  argp_program_version =
      "ft_ping (42 Advanced) 1.0\nWritten by Mario Martin Moreno";

  args.args_doc = "HOST ...";
  args.doc = "Send ICMP ECHO_REQUEST packets to network hosts.\vThe program "
             "requires superuser privileges due to raw packets.";
  args.options = options;

  argp_parse(&args, argc, argv, 0, &iarghost, &ping);

  return 0;
}
