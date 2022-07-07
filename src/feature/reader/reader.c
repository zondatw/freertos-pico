#include <FreeRTOS.h>
#include <queue.h>
#include <stdbool.h>
#include <stdio.h>
#include <task.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "logger.h"
#include "mfrc522.h"

#define SPI_PORT spi0

#define PIN_SCK 2
#define PIN_MOSI 3
#define PIN_MISO 4
#define PIN_RST 0
#define PIN_CS 1

int mfrc522_init(int32_t baudrate);
int mfrc522_reset();

int mfrc522_antenna_on();
int mfrc522_antenna_off();

int mfrc522_set_bit_mask(uint8_t reg, uint8_t mask);
int mfrc522_clear_bit_mask(uint8_t reg, uint8_t mask);

int mfrc522_write_reg(uint8_t reg, uint8_t value);
int mfrc522_read_reg(uint8_t reg, uint8_t *buf, size_t len);


static inline void spi_cs_select();
static inline void spi_cs_deselect();

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
}

int mfrc522_init(int32_t baudrate)
{
    logger_info(uart1, "[MFRC522 Init] start\r\n");
    stdio_init_all();

    logger_info(uart1, "[MFRC522 Init] spi init\r\n");
    spi_init(SPI_PORT, baudrate);

    spi_set_format(
        SPI_PORT,        // SPI instance
        8,               // Number of bits per transfer
        SPI_CPOL_0,      // Polarity (CPOL)
        SPI_CPHA_0,      // Phase (CPHA)
        SPI_MSB_FIRST);

    logger_info(uart1, "[MFRC522 Init] gpio seting\r\n");
    gpio_init(PIN_CS);
    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST, GPIO_OUT);
    gpio_set_dir(PIN_CS, GPIO_OUT);

    gpio_put(PIN_RST, 0);
    spi_cs_deselect();
    gpio_put(PIN_RST, 1);

    logger_info(uart1, "[MFRC522 Init] spi seting\r\n");
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);


    logger_info(uart1, "[MFRC522 Init] init command start\r\n");
    // init command
    mfrc522_reset();
    mfrc522_write_reg(T_MODE_REG, 0x8D);
    mfrc522_write_reg(T_PRESCALER_REG, 0x3E);
    mfrc522_write_reg(T_RELOAD_REG_L, 30);
    mfrc522_write_reg(T_RELOAD_REG_H, 0);
    mfrc522_write_reg(TX_AUTO_REG, 0x40);
    mfrc522_write_reg(MODE_REG, 0x3D);
    mfrc522_antenna_on();

    return 0;
}

int mfrc522_reset()
{
    mfrc522_write_reg(COMMAND_REG, PCD_RESETPHASE);
    return 0;
}

int mfrc522_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t msg[2] = {(reg << 1) & 0x7e, value};

    spi_cs_select();

    spi_write_blocking(SPI_PORT, msg, 2);

    spi_cs_deselect();
    return 0;
}

int mfrc522_read_reg(uint8_t reg, uint8_t *buf, size_t len)
{
    uint8_t mag = ((reg << 1) & 0x7e) | 0x80;

    spi_cs_select();

    spi_write_blocking(SPI_PORT, &mag, 1);
    spi_read_blocking(SPI_PORT, 0, buf, len);

    spi_cs_deselect();
    return 0;
}

int mfrc522_set_bit_mask(uint8_t reg, uint8_t mask)
{
    uint8_t buf[1];
    mfrc522_read_reg(reg, buf, 1);
    mfrc522_write_reg(reg, buf[0] | mask);
    return 0;
}

int mfrc522_clear_bit_mask(uint8_t reg, uint8_t mask)
{
    uint8_t buf[1];
    mfrc522_read_reg(reg, buf, 1);
    mfrc522_write_reg(reg, buf[0] & (~mask));
    return 0;
}

int mfrc522_antenna_on()
{
    logger_info(uart1, "[MFRC522 antenna on] start\r\n");
    uint8_t buf[1];
    mfrc522_read_reg(TX_CONTROL_REG, buf, 1);
    // buf[0] == 128
    logger_info(uart1, "[MFRC522 antenna on] read: %d\r\n", buf[0]);
    if (!(buf[0] & 0x03)) {
        mfrc522_set_bit_mask(TX_CONTROL_REG, 0x03);
    }
    return 0;
}

int mfrc522_antenna_off()
{
    return mfrc522_clear_bit_mask(TX_CONTROL_REG, 0x03);
}

static inline void spi_cs_select()
{
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void spi_cs_deselect()
{
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 1);
    asm volatile("nop \n nop \n nop");
}
