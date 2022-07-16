#include <FreeRTOS.h>
#include <queue.h>
#include <stdbool.h>
#include <stdio.h>
#include <task.h>
#include "pico/stdlib.h"

#include "logger.h"
#include "mfrc522.h"

// task
void reader_task(void *p);

int main()
{
    uart_init(uart1, 115200);
    stdio_init_all();
    logger_set_level(LOGGER_LEVEL_DEBUG);

    // Set the GPIO pin mux to the UART 1 - 0 is TX, 1 is RX
    // Set the GPIO pin mux to the UART 2 - 8 is TX, 9 is RX
    gpio_set_function(8, GPIO_FUNC_UART);
    gpio_set_function(9, GPIO_FUNC_UART);

    xTaskCreate(reader_task, "Reader task", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    while (true)
        ;
}


void reader_task(void *p)
{
    vTaskDelay(pdMS_TO_TICKS(2000));
    mfrc522_init(1000000);

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(200));
        uint8_t status = mfrc522_request(PICC_REQIDL);
        if (status == MIFARE_OK) {
            logger_info(uart1, "[Reader Task] Card detected\r\n");

            uint8_t back_data[16];
            uint8_t back_bits;
            status = mfrc522_anticoll(back_data, &back_bits);
            if (status == MIFARE_OK) {
                logger_info(uart1, "[Reader Task] UID: ");
                int8_t back_data_len = (int8_t) (back_bits / 8);
                for (int8_t index = back_data_len - 2; index >= 0; --index) {
                    logger_info(uart1, "%02X", back_data[index]);
                }
                logger_info(uart1, "\r\n");
            }
        } else {
            continue;
        }
    }
}
