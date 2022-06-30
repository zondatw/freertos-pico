#include <stdio.h>

#include "logger.h"

static int32_t logger_level = LOGGER_LEVEL_NOTSET;

#define debug_log()                     \
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

void logger_set_level(int32_t level)
{
    logger_level = level;
}

int32_t logger_log(int32_t level, uart_inst_t *uart, const char *fmt, ...)
{
    if (level < logger_level) {
        return -1;
    }

    int32_t ret;
    debug_log();
    return ret;
}