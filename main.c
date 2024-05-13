#include "ft_ping.h"
#include "parser.h"
#include "utils.h"

t_ft_ping ping;

int main(int argc, char **argv) {
  load_arguments(argc, argv);
  main_loop();
  terminate(0, NULL);
}
