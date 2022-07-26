#include <FreeRTOS.h>
#include <stdbool.h>
#include <stdio.h>
#include <task.h>
#include <hardware/flash.h>
#include "pico/stdlib.h"


#include "logger.h"
 
// We're going to erase and reprogram a region 256k from the start of flash.
// Once done, we can access this at XIP_BASE + 256k.
#define FLASH_TARGET_OFFSET (256 * 1024)
 
const uint8_t *flash_target_contents = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
 
void print_buf(const uint8_t *buf, size_t len) {
    logger_info(uart1, "len: %d\r\n", len);
    for (size_t i = 0; i < len; ++i) {
        logger_info(uart1, "%02x", buf[i]);
        if (i % 16 == 15) {
            logger_info(uart1, "\r\n");
        } else {
            logger_info(uart1, " ");
        }
    }
    logger_info(uart1, "------------------\r\n");
}

int main()
{
    uart_init(uart1, 115200);
    stdio_init_all();
    logger_set_level(LOGGER_LEVEL_DEBUG);

    // Set the GPIO pin mux to the UART 1 - 0 is TX, 1 is RX
    // Set the GPIO pin mux to the UART 2 - 8 is TX, 9 is RX
    gpio_set_function(8, GPIO_FUNC_UART);
    gpio_set_function(9, GPIO_FUNC_UART);

    sleep_ms(5000);
    logger_info(uart1, "Start\r\n");
    logger_info(uart1, "Read back target region:\r\n");
    print_buf(flash_target_contents, FLASH_PAGE_SIZE);
    
    uint8_t random_data[FLASH_PAGE_SIZE];
    for (int i = 0; i < FLASH_PAGE_SIZE; ++i)
        random_data[i] = rand() >> 16;
 
    logger_info(uart1, "Generated random data:\r\n");
    print_buf(random_data, FLASH_PAGE_SIZE);
    logger_info(uart1, "after Generated random data:\r\n");
 
    // Note that a whole number of sectors must be erased at a time.
    logger_info(uart1, "Erasing target region...\r\n");
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    logger_info(uart1, "Done. Read back target region:\r\n");
    print_buf(flash_target_contents, FLASH_PAGE_SIZE);
 
    logger_info(uart1, "Programming target region...\r\n");
    flash_range_program(FLASH_TARGET_OFFSET, random_data, FLASH_PAGE_SIZE);
    logger_info(uart1, "Done. Read back target region:\r\n");
    print_buf(flash_target_contents, FLASH_PAGE_SIZE);
 
    bool mismatch = false;
    for (int i = 0; i < FLASH_PAGE_SIZE; ++i) {
        if (random_data[i] != flash_target_contents[i])
            mismatch = true;
    }
    if (mismatch) {
        logger_info(uart1, "Programming failed!\r\n");
    }
    else {
        logger_info(uart1, "Programming successful!\r\n");
    }

    vTaskStartScheduler();
    while (true)
        ;
}
