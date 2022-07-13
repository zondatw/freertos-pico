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

mifare_status_t mfrc522_request(uint8_t req_mode);
mifare_status_t mfrc522_to_card(uint8_t cmd,
                                uint8_t *buf,
                                uint8_t len,
                                uint8_t *back_data,
                                uint8_t *back_bits);
mifare_status_t mfrc522_anticoll(uint8_t *back_data,
                                 uint8_t *back_bits);

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
        uint8_t status = mfrc522_request(PICC_REQIDL);
        if (status == MIFARE_OK) {
            logger_info(uart1, "[Reader Task] Card detected\r\n");

            uint8_t back_data[16];
            uint8_t back_bits;
            status = mfrc522_anticoll(back_data, &back_bits);
            if (status == MIFARE_OK) {
                logger_info(uart1, "[Reader Task] UID: ");
                int8_t back_data_len = (int8_t)(back_bits / 8);
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
    mfrc522_write_reg(MRFC522_T_MODE_REG, 0x8D);
    mfrc522_write_reg(MRFC522_T_PRESCALER_REG, 0x3E);
    mfrc522_write_reg(MRFC522_T_REALOD_L_REG, 30);
    mfrc522_write_reg(MRFC522_T_RELOAD_H_REG, 0);
    mfrc522_write_reg(MRFC522_TX_ASK_REG, 0x40);
    mfrc522_write_reg(MRFC522_MODE_REG, 0x3D);
    mfrc522_antenna_on();

    return 0;
}

int mfrc522_reset()
{
    mfrc522_write_reg(MRFC522_COMMAND_REG, PCD_SOFT_RESET);
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
    mfrc522_read_reg(MRFC522_TX_CONTROL_REG, buf, 1);
    logger_info(uart1, "[MFRC522 antenna on] read: %d\r\n", buf[0]);
    if (!(buf[0] & 0x03)) {
        mfrc522_set_bit_mask(MRFC522_TX_CONTROL_REG, 0x03);
    }
    return 0;
}

int mfrc522_antenna_off()
{
    return mfrc522_clear_bit_mask(MRFC522_TX_CONTROL_REG, 0x03);
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

mifare_status_t mfrc522_request(uint8_t req_mode)
{
    logger_debug(uart1, "[MFRC522 request] start\r\n");
    mfrc522_write_reg(MRFC522_BIT_FRAMING_REG, 0x07);

    uint8_t tag_type[1] = {req_mode};
    size_t tag_type_len = sizeof(tag_type) / sizeof(tag_type[0]);

    uint8_t back_data[16];
    uint8_t back_bits;
    mifare_status_t status =
        mfrc522_to_card(PCD_TRANSCETIVE, tag_type, tag_type_len, back_data, &back_bits);
    if ((status != MIFARE_OK) | (back_bits != 0x10)) {
        return MIFARE_ERR;
    }
    return MIFARE_OK;
}


mifare_status_t mfrc522_anticoll(uint8_t *back_data,
                                 uint8_t *back_bits)
{
    logger_debug(uart1, "[MFRC522 anticoll] start\r\n");
    mfrc522_write_reg(MRFC522_BIT_FRAMING_REG, 0x00);

    uint8_t tag_type[2] = {PICC_ANTICOLL, 0x20};
    size_t tag_type_len = sizeof(tag_type) / sizeof(tag_type[0]);

    mifare_status_t status =
        mfrc522_to_card(PCD_TRANSCETIVE, tag_type, tag_type_len, back_data, back_bits);

    if (status != MIFARE_OK) {
        uint8_t serial_num_chk = 0;
        for (uint8_t i = 0; i <= 3; ++i) {
            serial_num_chk ^= back_data[i];
        }
        if (serial_num_chk != back_data[4]) {
            return MIFARE_ERR;
        }
    }

    return status;
}

mifare_status_t mfrc522_to_card(uint8_t cmd,
                                uint8_t *buf,
                                uint8_t len,
                                uint8_t *back_data,
                                uint8_t *back_bits)
{
    mifare_status_t status = MIFARE_ERR;
    uint8_t irq_en = 0x00;
    uint8_t wait_irq = 0x00;
    uint8_t last_bits = 0x00;
    uint8_t n = 0;

    if (cmd == PCD_MF_AUTHENT) {
        irq_en = 0x12;
        wait_irq = 0x10;
    } else if (cmd == PCD_TRANSCETIVE) {
        irq_en = 0x77;
        wait_irq = 0x30;
    }

    mfrc522_write_reg(0x02, irq_en | 0x80);
    mfrc522_clear_bit_mask(0x04, 0x80);
    mfrc522_set_bit_mask(0x0A, 0x80);

    mfrc522_write_reg(MRFC522_COMMAND_REG, PCD_IDLE);

    for (int i = 0; i < len; ++i) {
        mfrc522_write_reg(MRFC522_FIFO_DATA_REG, buf[i]);
    }

    mfrc522_write_reg(MRFC522_COMMAND_REG, cmd);

    if (cmd == PCD_TRANSCETIVE) {
        mfrc522_set_bit_mask(MRFC522_BIT_FRAMING_REG, 0x80);
    }

    int index = 2000;
    do {
        mfrc522_read_reg(0x04, &n, 1);
        index--;
    } while ((index != 0) && !(n & 0x01) && !(n & wait_irq));

    mfrc522_clear_bit_mask(MRFC522_BIT_FRAMING_REG, 0x80);

    if (index) {
        uint8_t _buf;
        mfrc522_read_reg(MRFC522_ERROR_REG, &_buf, 1);
        if ((_buf & 0x1B) == 0x00) {
            status = MIFARE_OK;

            if (n & irq_en & 0x01) {
                status = MIFARE_NOTAGERR;
            } else if (cmd == PCD_TRANSCETIVE) {
                mfrc522_read_reg(MRFC522_FIFO_LEVEL_REG, &n, 1);
                mfrc522_read_reg(MRFC522_CONTROL_REG, &last_bits, 1);
                last_bits &= 0x07;

                if (last_bits != 0) {
                    *back_bits = (n - 1) * 8 + last_bits;
                } else {
                    *back_bits = n * 8;
                }

                if (n == 0) {
                    n = 1;
                } else if (n > 16) {
                    n = 16;
                }

                logger_info(uart1, "[MFRC522 to card] ");
                for (int i = 0; i < n; ++i) {
                    mfrc522_read_reg(MRFC522_FIFO_DATA_REG, &back_data[i], 1);

                    logger_info(uart1, "%d ", back_data[i]);
                }
                logger_info(uart1, "\r\n");
            }
        } else {
            status = MIFARE_ERR;
        }
    }

    return status;
}