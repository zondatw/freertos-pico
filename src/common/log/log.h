#ifndef COMMON_LOG_H
#define COMMON_LOG_H

#include "pico/stdlib.h"
#include <stdarg.h>

int32_t debug_log(uart_inst_t *uart, const char *fmt, ...);

#endif // COMMON_LOG_H