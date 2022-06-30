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

#define logger_notset(...) logger(LOG_NOTSET, __VA_ARGS__);
#define logger_debug(...) logger(LOG_DEBUG, __VA_ARGS__);
#define logger_info(...) logger(LOG_INFO, __VA_ARGS__);
#define logger_warning(...) logger(LOG_WARNING, __VA_ARGS__);
#define logger_error(...) logger(LOG_ERROR, __VA_ARGS__);
#define logger_critical(...) logger(LOG_CRITICAL, __VA_ARGS__);

void set_level(int32_t level);
int32_t debug_log(uart_inst_t *uart, const char *fmt, ...);
int32_t logger(int32_t level, uart_inst_t *uart, const char *fmt, ...);

#endif  // COMMON_LOG_H