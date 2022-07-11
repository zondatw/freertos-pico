#include <FreeRTOS.h>
#include <queue.h>
#include <stdbool.h>
#include <stdio.h>
#include <task.h>
#include "hardware/spi.h"
#include "pico/stdlib.h"

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

uint8_t mfrc522_request(uint8_t req_mode);
uint8_t mfrc522_to_card(uint8_t cmd, uint8_t *buf, uint8_t len, uint8_t *back_data, uint8_t *back_len);

static inline void mfrc522_spi_cs_select();
static inline void mfrc522_spi_cs_deselect();

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
        mfrc522_request(PICC_REQIDL);
    }
}

int mfrc522_init(int32_t baudrate)
{
    logger_info(uart1, "[MFRC522 Init] start\r\n");
    stdio_init_all();

    logger_info(uart1, "[MFRC522 Init] spi init\r\n");
    spi_init(SPI_PORT, baudrate);

    spi_set_format(SPI_PORT,    // SPI instance
                   8,           // Number of bits per transfer
                   SPI_CPOL_0,  // Polarity (CPOL)
                   SPI_CPHA_0,  // Phase (CPHA)
                   SPI_MSB_FIRST);

    logger_info(uart1, "[MFRC522 Init] gpio seting\r\n");
    gpio_init(PIN_CS);
    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST, GPIO_OUT);
    gpio_set_dir(PIN_CS, GPIO_OUT);

    gpio_put(PIN_RST, 0);
    mfrc522_spi_cs_deselect();
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
    uint8_t msg[2] = {(reg << 1) & 0x7e,  // SPI address byte
                      value};

    mfrc522_spi_cs_select();

    spi_write_blocking(SPI_PORT, msg, 2);

    mfrc522_spi_cs_deselect();
    return 0;
}

int mfrc522_read_reg(uint8_t reg, uint8_t *buf, size_t len)
{
    uint8_t mag = ((reg << 1) & 0x7e) | 0x80;  // SPI address byte

    mfrc522_spi_cs_select();

    spi_write_blocking(SPI_PORT, &mag, 1);
    spi_read_blocking(SPI_PORT, 0, buf, len);

    mfrc522_spi_cs_deselect();
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

static inline void mfrc522_spi_cs_select()
{
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void mfrc522_spi_cs_deselect()
{
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 1);
    asm volatile("nop \n nop \n nop");
}

uint8_t mfrc522_request(uint8_t req_mode)
{
    logger_debug(uart1, "[MFRC522 request] start\r\n");
    mfrc522_write_reg(BIT_FRAMING_REG, 0x07);

    uint8_t tag_type[1] = {req_mode};

    uint8_t back_data[16];
    uint8_t back_bits;
    uint8_t status = mfrc522_to_card(PCD_TRANSCETIVE, tag_type, 1, back_data, &back_bits);
    if ((status != MI_OK) | (back_bits != 0x10)) {
        return MI_ERR;
    }
    return MI_OK;
}

uint8_t mfrc522_to_card(uint8_t cmd, uint8_t *buf, uint8_t len, uint8_t *back_data, uint8_t *back_len)
{
    uint8_t status = MI_ERR;
    uint8_t irq_en = 0x00;
    uint8_t wait_irq = 0x00;
    uint8_t last_bits = 0x00;
    uint8_t n = 0;

    if (cmd == PCD_AUTHENT) {
        irq_en = 0x12;
        wait_irq = 0x10;
    } else if (cmd == PCD_TRANSCETIVE) {
        irq_en = 0x77;
        wait_irq = 0x30;
    }

    mfrc522_write_reg(0x02, irq_en | 0x80);
    mfrc522_clear_bit_mask(0x04, 0x80);
    mfrc522_set_bit_mask(0x0A, 0x80);

    mfrc522_write_reg(COMMAND_REG, PCD_IDLE);

    for (int i = 0; i < len; ++i) {
        mfrc522_write_reg(FIFO_DATA_REG, buf[i]);
    }

    mfrc522_write_reg(COMMAND_REG, cmd);

    if (cmd == PCD_TRANSCETIVE) {
        mfrc522_set_bit_mask(BIT_FRAMING_REG, 0x80);
    }

    int index = 2000;
    do {
        mfrc522_read_reg(0x04, &n, 1);
        index--;
    } while ((index != 0) && !(n & 0x01) && !(n & wait_irq));

    mfrc522_clear_bit_mask(BIT_FRAMING_REG, 0x80);

    if (index) {
        uint8_t _buf;
        mfrc522_read_reg(ERROR_REG, &_buf, 1);
        if ((_buf & 0x1B) == 0x00) {
            status = MI_OK;

            if (n & irq_en & 0x01) {
                status = MI_NOTAGERR;
            } else if (cmd == PCD_TRANSCETIVE) {
                mfrc522_read_reg(FIFO_LEVEL_REG, &n, 1);
                mfrc522_read_reg(CONTROL_REG, &last_bits, 1);
                last_bits &= 0x07;

                if (last_bits != 0) {
                    *back_len = (n - 1) * 8 + last_bits;
                } else {
                    *back_len = n * 8;
                }

                if (n == 0) {
                    n = 1;
                } else if (n > 16) {
                    n = 16;
                }

                logger_info(uart1, "[MFRC522 to card] ");
                for (int i = 0; i < n; ++i) {
                    mfrc522_read_reg(FIFO_DATA_REG, &back_data[i], 1);
                    
                    logger_info(uart1, "%d ", back_data[i]);
                }
                logger_info(uart1, "\r\n");
            }
        } else {
            status = MI_ERR;
        }
    }

    return status;
}