#include <modbus/modbus-tcp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

extern modbus_t * ctx ;
extern char const *service ;
extern char const *tcp_host ;
extern uint16_t unit_id ;
extern uint16_t addr ;
extern uint16_t n_registers ;
extern bool use_hex ;
extern int timeout_ms ;
extern bool verbose_mode ;

// modules must provide these
extern int handle_request(void);
extern int print_manual(FILE *file, char const * argv0);
