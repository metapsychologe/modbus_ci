#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "modbus_cli.h"

#define TO_STR(s) STR(s)
#define STR(s) #s

#if (defined READ_COILS) || (defined READ_INPUT_BITS)
typedef uint8_t register_type_t;
#elif (defined READ_HOLDING_REGISTERS) || (defined READ_INPUT_REGISTERS)
typedef uint16_t register_type_t;
#endif

static char const *service = TO_STR(MODBUS_TCP_DEFAULT_PORT);
static char const *tcp_host = "localhost";
static uint16_t unit_id = 0xff;
static uint16_t addr = 0;
static uint16_t n_registers = 1;
static bool use_hex = false;
static int timeout_ms = 1000;
static bool verbose_mode = false;

static int print_manual(FILE *file, char const argv0[]) {
  return fprintf(
      file,
      "%s is a simple modbus client that will try to read "
      "registers and quit.\n"
      "modbus_read_register is running on libmodbus "
      "Version " LIBMODBUS_VERSION_STRING
      "\n"
      "Usage: modbus_read_register -h 192.168.0.1 -p 602 -u 1 -s 40000 -n 125 "
      "-x\n"
      "will read 125 registers starting from address 40000 with from "
      "192.168.0.1:602 with unit id 1\n\n"
      "-p --port host port, default is 502\n"
      "-h --host server host name defaults to localhost\n"
      "-u --unit_id used unit id to use, defaults to 0xff as suggested by "
      "ModbusTCP\n"
      "-s --address first modbus register address to read from\n"
      "-n number of registers to read\n"
      "-v verbose mode\n"
      "-x --hex hexadecimal output\n"
      "-H --help print this help message\n",
      argv0);
}

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

static int print(register_type_t const *data, size_t n_data) {
#if (defined READ_COILS) || (defined READ_INPUT_BITS)
  char const *format_string = "%d\t";
#elif (defined READ_HOLDING_REGISTERS) || (defined READ_INPUT_REGISTERS)
  char const *format_string = use_hex ? ("%" PRIx16 "\t") : ("%" PRIu16 "\t");
#endif
  for (size_t i = 0; i < n_data; ++i) {
    int result = printf(format_string, data[i]);
    if (result <= 0) return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

static int handle_request(modbus_t *ctx) {
  uint16_t n_read_registers = 0;

  do {
    uint16_t to_read_registers = n_registers - n_read_registers;
    if (to_read_registers > MODBUS_MAX_READ_REGISTERS) {
      to_read_registers = MODBUS_MAX_READ_REGISTERS;
    }
    n_read_registers += to_read_registers;

    register_type_t read_regs[MODBUS_MAX_READ_REGISTERS];
    int read_res =
#if defined READ_COILS
        modbus_read_bits(ctx, addr, to_read_registers, read_regs);
#elif defined READ_INPUT_BITS
        modbus_read_input_bits(ctx, addr, to_read_registers, read_regs);
#elif defined READ_HOLDING_REGISTERS
        modbus_read_registers(ctx, addr, to_read_registers, read_regs);
#elif defined READ_INPUT_REGISTERS
        modbus_read_input_registers(ctx, addr, to_read_registers, read_regs);
#else
#error define read function
#endif

    if (read_res < 0) {
      (void)fprintf(stderr, "Error: %s\n", modbus_strerror(errno));
      return EXIT_FAILURE;
    }

    int result = print(read_regs, n_registers);
    if (EXIT_SUCCESS != result) return result;
  } while (n_read_registers < n_registers);

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  read_arguments(argc, argv);

  if (verbose_mode) {
    int res =
        printf("Reading %d registers starting from Addres %d from %s:%s\n",
               n_registers, addr, tcp_host, service);
    if (res <= 0) exit(EXIT_FAILURE);
  }

  modbus_t *ctx = get_client_context(tcp_host, service, unit_id, verbose_mode);
  if (ctx) {
    return handle_request(ctx);
  } else {
    return EXIT_FAILURE;
  }
}
