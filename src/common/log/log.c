
#include "log.h"


int32_t debug_log(uart_inst_t *uart, const char *fmt, ...)
{
    static char buf[1024];
    int32_t ret;
    va_list args;

    va_start(args, fmt);
    ret = vsprintf(buf, fmt, args);
    va_end(args);

    printf("%s", buf);
    uart_puts(uart, buf);
    return ret;
}