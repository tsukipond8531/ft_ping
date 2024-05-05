#include "ft_ping.h"
#include "utils.h"
#include <argp.h>
#include <stdlib.h>
#include <string.h>

extern t_ft_ping ping;

static int parse_argument(int key, char *arg, struct argp_state *state) {
  char *argend = NULL;

  switch (key) {
  case 'c':
    ping.settings.count = strtol(arg, &argend, 10);
    ping.settings.flags = SET_COUNT_FLAG(ping.settings.flags);
    break;
  case 'i':
    ping.settings.interval = strtol(arg, &argend, 10);
    ping.settings.flags = SET_INTERVAL_FLAG(ping.settings.flags);
    break;
  case 0x04:
    ping.settings.ttl = strtol(arg, &argend, 10);
    ping.settings.flags = SET_TTL_FLAG(ping.settings.flags);
    break;
  case 'T':
    ping.settings.tos = strtol(arg, &argend, 10);
    ping.settings.flags = SET_TOS_FLAG(ping.settings.flags);
    break;
  case 'v':
    ping.settings.verbose = true;
    ping.settings.flags = SET_VERBOSE_FLAG(ping.settings.flags);
    break;
  case 'w':
    ping.settings.timeout = strtol(arg, &argend, 10);
    ping.settings.flags = SET_TIMEOUT_FLAG(ping.settings.flags);
    break;
  case 'W':
    ping.settings.linger = strtol(arg, &argend, 10);
    ping.settings.flags = SET_LINGER_FLAG(ping.settings.flags);
    break;
  case 'l':
    ping.settings.preload = strtol(arg, &argend, 10);
    ping.settings.flags = SET_PRELOAD_FLAG(ping.settings.flags);
    break;
  case 'p':
    ping.settings.pattern = arg;
    ping.settings.flags = SET_PATTERN_FLAG(ping.settings.flags);
    break;
  case ARGP_KEY_ARG:
    add_host(arg);
    break;
  case ARGP_KEY_END:
    if (state->arg_num < 1)
      argp_usage(state);
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  if (argend && *argend) {
    fprintf(stderr, "Invalid value for argument: '%s'!\n", argend);
    argp_usage(state);
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

void load_arguments(int argc, char **argv) {
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
  args.parser = parse_argument;

  if (argp_parse(&args, argc, argv, 0, NULL, NULL) != 0)
    terminate(1, "Error parsing options. Try using --help");
}
