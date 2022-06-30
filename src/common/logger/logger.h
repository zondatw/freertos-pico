#ifndef COMMON_LOGGER_H
#define COMMON_LOGGER_H

#include <stdarg.h>
#include "pico/stdlib.h"

enum {
    LOGGER_LEVEL_NOTSET = 0,
    LOGGER_LEVEL_DEBUG = 10,
    LOGGER_LEVEL_INFO = 20,
    LOGGER_LEVEL_WARNING = 30,
    LOGGER_LEVEL_ERROR = 40,
    LOGGER_LEVEL_CRITICAL = 50,
};

#define logger_notset(...) logger_log(LOGGER_LEVEL_NOTSET, __VA_ARGS__);
#define logger_debug(...) logger_log(LOGGER_LEVEL_DEBUG, __VA_ARGS__);
#define logger_info(...) logger_log(LOGGER_LEVEL_INFO, __VA_ARGS__);
#define logger_warning(...) logger_log(LOGGER_LEVEL_WARNING, __VA_ARGS__);
#define logger_error(...) logger_log(LOGGER_LEVEL_ERROR, __VA_ARGS__);
#define logger_critical(...) logger_log(LOGGER_LEVEL_CRITICAL, __VA_ARGS__);

void logger_set_level(int32_t level);
int32_t logger_log(int32_t level, uart_inst_t *uart, const char *fmt, ...);

#endif  // COMMON_LOGGER_H