#include <stdio.h>

#include "logger.h"

static int32_t log_level = LOG_NOTSET;

#define uart_debug_log()                \
    do {                                \
        static char buf[1024];          \
        va_list args;                   \
                                        \
        va_start(args, fmt);            \
        ret = vsprintf(buf, fmt, args); \
        va_end(args);                   \
                                        \
        printf("%s", buf);              \
        uart_puts(uart, buf);           \
    } while (0)

int32_t debug_log(uart_inst_t *uart, const char *fmt, ...)
{
    int32_t ret;
    uart_debug_log();
    return ret;
}

void set_level(int32_t level)
{
    log_level = level;
}

int32_t logger(int32_t level, uart_inst_t *uart, const char *fmt, ...)
{
    if (level < log_level) {
        return -1;
    }

    int32_t ret;
    uart_debug_log();
    return ret;
}