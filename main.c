#include "main.h"

#include <errno.h>
#include <getopt.h>
#include <stdlib.h>

#define TO_STR(s) STR(s)
#define STR(s) #s

modbus_t *ctx = NULL;

char const *service = TO_STR(MODBUS_TCP_DEFAULT_PORT);
char const *tcp_host = "localhost";
uint16_t unit_id = 0xff;
uint16_t addr = 0;
uint16_t n_registers = 1;
bool use_hex = false;
int timeout_ms = 1000;
bool verbose_mode = false;

static void free_ctx(void) { modbus_free(ctx); }
static void close_ctx(void) { modbus_close(ctx); }

static struct option const longoptions[] = {
    {"port", required_argument, NULL, 'p'},
    {"host", required_argument, NULL, 'h'},
    {"unit_id", no_argument, NULL, 'u'},
    {"address", required_argument, NULL, 's'},
    {"hex", no_argument, NULL, 'x'},
    {"help", no_argument, NULL, 'H'},
    {0, 0, 0, 0}};
static char const shortoptions[] = "p:h:u:s:n:xv";

static void read_arguments(int argc, char *argv[]) {
  int getopt_result;
  do {
    getopt_result = getopt_long(argc, argv, shortoptions, longoptions, NULL);
    switch (getopt_result) {
      case 'p':
        service = optarg;
        break;
      case 'h':
        tcp_host = optarg;
        break;
      case 'u':
        unit_id = atoi(optarg);
        break;
      case 's':
        addr = atoi(optarg);
        break;
      case 'n':
        n_registers = atoi(optarg);
        break;
      case 'x':
        use_hex = true;
        break;
      case 't':
        timeout_ms = atoi(optarg);
        break;
      case 'v':
        verbose_mode = true;
        break;
      case 0:
        break;
      case 'H':
        exit(print_manual(stdout, argv[0]) > 0 ? EXIT_SUCCESS : EXIT_FAILURE);
      default:
        (void)print_manual(stderr, argv[0]);
        exit(EXIT_FAILURE);
      case -1:
        return;
    }
  } while (getopt_result >= 0);
}


int main(int argc, char *argv[]) {
  read_arguments(argc, argv);

  ctx = modbus_new_tcp_pi(tcp_host, service);
  if (atexit(free_ctx)) {
    perror("Registering function with atexit");
    return EXIT_FAILURE;
  }

  if (verbose_mode) {
    int res =
        printf("Reading %d registers starting from Addres %d from %s:%s\n",
               n_registers, addr, tcp_host, service);
    if (res <= 0) exit(EXIT_FAILURE);

    res = modbus_set_debug(ctx, TRUE);
    if (res < 0) {
      (void)fprintf(stderr, "Error: %s\n", modbus_strerror(errno));
      return EXIT_FAILURE;
    }
  }

  if (!ctx) {
    (void)fprintf(stderr, "Error: %s\n", modbus_strerror(errno));
    return EXIT_FAILURE;
  }

  int set_slave_ret = modbus_set_slave(ctx, unit_id);
  if (set_slave_ret < 0) {
    (void)fprintf(stderr, "Error: %s\n", modbus_strerror(errno));
    return EXIT_FAILURE;
  }

  int connect_res = modbus_connect(ctx);
  if (connect_res < 0) {
    (void)fprintf(stderr, "Error: %s\n", modbus_strerror(errno));
    return EXIT_FAILURE;
  }

  if (atexit(close_ctx)) {
    (void)perror("Registering function with atexit");
    return EXIT_FAILURE;
  }

  return handle_request();
}
