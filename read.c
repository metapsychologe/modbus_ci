#include <errno.h>
#include <inttypes.h>

#include "main.h"
#include "stdlib.h"

#if (defined READ_COILS) || (defined READ_INPUT_BITS)
typedef uint8_t register_type_t;
#elif (defined READ_HOLDING_REGISTERS) || (defined READ_INPUT_REGISTERS)
typedef uint16_t register_type_t;
#endif

#define ELMENT_SEPERATOR "\t"

int print_manual(FILE *file, char const *argv0) {
  (void)argv0;
  return fputs(
      "modbus_read_register is a simple modbus client that will try to read "
      "registers and quit.\n"
      "modbus_read_register is running on libmodbus "
      "Version " LIBMODBUS_VERSION_STRING
      "\n"
      "Usage: modbus_read_register\n",
      file);
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

int handle_request(void) {
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
