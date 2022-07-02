#include <FreeRTOS.h>
#include <queue.h>
#include <stdbool.h>
#include <stdio.h>
#include <task.h>
#include "pico/stdlib.h"

#include "logger.h"

void sender_1_task(void *p);
void sender_2_task(void *p);
void receiver_task(void *p);

static QueueSetHandle_t x_queue_set = NULL;
static QueueHandle_t x_queue1 = NULL, x_queue2 = NULL;

int main()
{
    uart_init(uart1, 115200);
    stdio_init_all();
    logger_set_level(LOGGER_LEVEL_DEBUG);

    // Set the GPIO pin mux to the UART 1 - 0 is TX, 1 is RX
    // Set the GPIO pin mux to the UART 2 - 8 is TX, 9 is RX
    gpio_set_function(8, GPIO_FUNC_UART);
    gpio_set_function(9, GPIO_FUNC_UART);

    x_queue1 = xQueueCreate(1, sizeof(char *));
    x_queue2 = xQueueCreate(1, sizeof(char *));

    x_queue_set = xQueueCreateSet(1 * 2);

    xQueueAddToSet(x_queue1, x_queue_set);
    xQueueAddToSet(x_queue2, x_queue_set);

    xTaskCreate(sender_1_task, "Queue set sender 1 task", 256, NULL, 1, NULL);
    xTaskCreate(sender_2_task, "Queue set sender 2 task", 256, NULL, 1, NULL);
    xTaskCreate(receiver_task, "Queue set recv task", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    while (true)
        ;
}

void sender_1_task(void *p)
{
    const char *msg = "Sender 1 message\r\n";
    const TickType_t ticks_to_wait = pdMS_TO_TICKS(100);
    while (true) {
        vTaskDelay(ticks_to_wait);
        logger_debug(uart1, "[Queue sender 1] send msg\r\n");
        xQueueSend(x_queue1, &msg, ticks_to_wait);
    }
}

void sender_2_task(void *p)
{
    const char *msg = "Sender 2 message\r\n";
    const TickType_t ticks_to_wait = pdMS_TO_TICKS(100);
    while (true) {
        vTaskDelay(ticks_to_wait);
        logger_debug(uart1, "[Queue sender 2] send msg\r\n");
        xQueueSend(x_queue1, &msg, ticks_to_wait);
    }
}

void receiver_task(void *p)
{
    QueueHandle_t x_queue_contain;
    char *msg;

    while (true) {
        x_queue_contain =
            (QueueHandle_t) xQueueSelectFromSet(x_queue_set, portMAX_DELAY);

        BaseType_t status = xQueueReceive(x_queue_contain, &msg, 0);
        if (status == pdTRUE) {
            logger_debug(uart1, "[Queue recv] recv success\r\n");
            logger_info(uart1, "[Queue recv] recv: %s\r\n", msg);
        } else {
            logger_warning(uart1, "[Queue recv] recv fail\r\n");
        }
    }
}
