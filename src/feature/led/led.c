#include <FreeRTOS.h>
#include <queue.h>
#include <stdbool.h>
#include <stdio.h>
#include <task.h>
#include "pico/stdlib.h"

#include "log.h"

void led_task(void *p);
void send_task(void *p);
void recv_task(void *p);

struct led_params {
    int gpio_pin;
    int on_delay;
    int off_delay;
};

struct queue_params {
    QueueHandle_t xqueue;
};

struct queue_item {
    int32_t id;
};

int main()
{
    uart_init(uart1, 115200);
    stdio_init_all();
    set_level(LOG_DEBUG);

    struct led_params params = {
        .gpio_pin = 9,
        .on_delay = 500,
        .off_delay = 500,
    };
    struct queue_params qparmas = {
        .xqueue = xQueueCreate(5, sizeof(struct queue_item))};

    // Set the GPIO pin mux to the UART 1 - 0 is TX, 1 is RX
    // Set the GPIO pin mux to the UART 2 - 8 is TX, 9 is RX
    gpio_set_function(8, GPIO_FUNC_UART);
    gpio_set_function(9, GPIO_FUNC_UART);


    if (qparmas.xqueue != NULL) {
        logger(LOG_INFO, uart1, "[Queue Init] create success\r\n");
        xTaskCreate(send_task, "Queue send task", 256, &qparmas, 1, NULL);
        xTaskCreate(recv_task, "Queue recv task", 256, &qparmas, 1, NULL);
    } else {
        logger(LOG_ERROR, uart1, "[Queue Init] create fail\r\n");
    }

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
        logger(LOG_DEBUG, uart1, "[LED] GPIO PIN %d ON\r\n", params->gpio_pin);
        vTaskDelay(pdMS_TO_TICKS(params->on_delay));

        gpio_put(params->gpio_pin, false);
        logger(LOG_DEBUG, uart1, "[LED] GPIO PIN %d OFF\r\n", params->gpio_pin);
        vTaskDelay(pdMS_TO_TICKS(params->off_delay));
    }
}

void recv_task(void *p)
{
    struct queue_item item;
    struct queue_params *qparmas = (struct queue_params *) p;
    const TickType_t ticks_to_wait = pdMS_TO_TICKS(100);

    while (true) {
        if (uxQueueMessagesWaiting(qparmas->xqueue) == 0) {
            // debug_log(uart1, "[Queue recv] recv empty\r\n");
            continue;
        }
        BaseType_t status =
            xQueueReceive(qparmas->xqueue, &item, ticks_to_wait);
        if (status == pdTRUE) {
            logger(LOG_DEBUG, uart1, "[Queue recv] recv success\r\n");
            logger(LOG_INFO, uart1, "[Queue recv] recv: %d\r\n", item.id);
        } else {
            logger(LOG_WARNING, uart1, "[Queue recv] recv fail\r\n");
        }
    }
}

void send_task(void *p)
{
    int32_t count = 0;
    struct queue_params *qparmas = (struct queue_params *) p;
    while (true) {
        struct queue_item item = {.id = count++};
        BaseType_t status = xQueueSendToBack(qparmas->xqueue, &item, 0);
        if (status != pdTRUE) {
            logger(LOG_WARNING, uart1, "[Queue send] send fail\r\n");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}