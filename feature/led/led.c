#include <FreeRTOS.h>
#include <stdbool.h>
#include <stdio.h>
#include <task.h>
#include "pico/stdlib.h"

void led_task(void *p);

struct led_params {
    int gpio_pin;
    int on_delay;
    int off_delay;
};

int main()
{
    uart_init(uart1, 115200);
    stdio_init_all();
    struct led_params params = {
        .gpio_pin = 9,
        .on_delay = 500,
        .off_delay = 500,
    };

    // Set the GPIO pin mux to the UART 1 - 0 is TX, 1 is RX
    // Set the GPIO pin mux to the UART 2 - 8 is TX, 9 is RX
    gpio_set_function(8, GPIO_FUNC_UART);
    gpio_set_function(9, GPIO_FUNC_UART);

    xTaskCreate(led_task, "LED Task 1", 256, &params, 1, NULL);

    vTaskStartScheduler();
    while (true)
        ;
}

void led_task(void *p)
{
    struct led_params *params = (struct led_params *) p;

    gpio_init(params->gpio_pin);
    gpio_set_dir(params->gpio_pin, GPIO_OUT);

    while (true) {
        gpio_put(params->gpio_pin, true);
        printf("[LED] GPIO PIN %d ON\n", params->gpio_pin);
        uart_puts(uart1, "[LED] ON\r\n");
        vTaskDelay(pdMS_TO_TICKS(params->on_delay));

        gpio_put(params->gpio_pin, false);
        printf("[LED] GPIO PIN %d OFF\n", params->gpio_pin);
        uart_puts(uart1, "[LED] OFF\r\n");
        vTaskDelay(pdMS_TO_TICKS(params->off_delay));
    }
}
