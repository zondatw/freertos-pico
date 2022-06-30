#ifndef COMMON_LOG_H
#define COMMON_LOG_H

#include <stdarg.h>
#include "pico/stdlib.h"

enum {
    LOG_NOTSET = 0,
    LOG_DEBUG = 10,
    LOG_INFO = 20,
    LOG_WARNING = 30,
    LOG_ERROR = 40,
    LOG_CRITICAL = 50,
};

void set_level(int32_t level);
int32_t debug_log(uart_inst_t *uart, const char *fmt, ...);
int32_t logger(int32_t level, uart_inst_t *uart, const char *fmt, ...);

#endif  // COMMON_LOG_H