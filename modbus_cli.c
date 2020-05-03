#include "modbus_cli.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

static modbus_t* ctx;

static void free_ctx(void) { modbus_free(ctx); }
static void close_ctx(void) { modbus_close(ctx); }

modbus_t* get_client_context(char const tcp_host[], char const service[],
                             uint8_t unit_id, bool verbose_mode) {
  if (ctx) {
    perror("context already built");
    return NULL;
  }

  ctx = modbus_new_tcp_pi(tcp_host, service);
  if (atexit(free_ctx)) {
    perror("Registering function with atexit");
    return NULL;
  }

  if (verbose_mode) {
    int res = modbus_set_debug(ctx, TRUE);
    if (res < 0) {
      (void)fprintf(stderr, "Error: %s\n", modbus_strerror(errno));
      return NULL;
    }
  }

  if (!ctx) {
    (void)fprintf(stderr, "Error: %s\n", modbus_strerror(errno));
    return NULL;
  }

  int set_slave_ret = modbus_set_slave(ctx, unit_id);
  if (set_slave_ret < 0) {
    (void)fprintf(stderr, "Error: %s\n", modbus_strerror(errno));
    return NULL;
  }

  int connect_res = modbus_connect(ctx);
  if (connect_res < 0) {
    (void)fprintf(stderr, "Error: %s\n", modbus_strerror(errno));
    return NULL;
  }

  if (atexit(close_ctx)) {
    (void)perror("Registering function with atexit");
    return NULL;
  }

  return ctx;
}
