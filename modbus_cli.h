#include <modbus/modbus-tcp.h>
#include <stdbool.h>
#include <stdint.h>

modbus_t* get_client_context(char const tcp_host[], char const service[],
                             uint8_t unit_id, bool verbose_mode);
