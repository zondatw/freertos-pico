#ifndef MRFC522_H
#define MRFC522_H

#include "logger.h"

// Command & status
// page 0
#define MRFC522_RESERVED_00 0x00
#define MRFC522_COMMAND_REG 0x01
#define MRFC522_COM_IEN_REG 0x02
#define MRFC522_DIV_IEN_REG 0x03
#define MRFC522_COM_IRQ_REG 0x04
#define MRFC522_DIV_IRQ_REG 0x04
#define MRFC522_ERROR_REG 0x06
#define MRFC522_STATUS_1_REG 0x07
#define MRFC522_STATUS_2_REG 0x08
#define MRFC522_FIFO_DATA_REG 0x09
#define MRFC522_FIFO_LEVEL_REG 0x0A
#define MRFC522_WATER_LEVEL_REG 0x0B
#define MRFC522_CONTROL_REG 0x0C
#define MRFC522_BIT_FRAMING_REG 0x0D
#define MRFC522_COLL_REG 0x0E
#define MRFC522_RESERVED_0F 0x0F

// page 1
#define MRFC522_RESERVED_10 0x10
#define MRFC522_MODE_REG 0x11
#define MRFC522_TX_MODE_REG 0x12
#define MRFC522_RX_MODE_REG 0x13
#define MRFC522_TX_CONTROL_REG 0x14
#define MRFC522_TX_ASK_REG 0x15
#define MRFC522_TX_SEL_REG 0x16
#define MRFC522_RX_SEL_REG 0x17
#define MRFC522_RX_THRESHOLD_REG 0x18
#define MRFC522_DEMOD_REG 0x19
#define MRFC522_RESERVED_1A 0x1A
#define MRFC522_RESERVED_1B 0x1B
#define MRFC522_MF_TX_REG 0x1C
#define MRFC522_MF_RX_REG 0x1D
#define MRFC522_RESERVED_1E 0x1E
#define MRFC522_SERIAL_SPEED_REG 0x1F

// page 2
#define MRFC522_RESERVED_20 0x20
#define MRFC522_CRC_RESULT_MSB_REG 0x21
#define MRFC522_CRC_RESULT_LSB_REG 0x22
#define MRFC522_RESERVED_23 0x23
#define MRFC522_MOD_WIDTH_REG 0x24
#define MRFC522_RESERVED_25 0x25
#define MRFC522_RF_CFG_REG 0x26
#define MRFC522_GS_N_REG 0x27
#define MRFC522_CW_GS_P_REG 0x28
#define MRFC522_MOD_GS_P_REG 0x29
#define MRFC522_T_MODE_REG 0x2A
#define MRFC522_T_PRESCALER_REG 0x2B
#define MRFC522_T_RELOAD_H_REG 0x2C
#define MRFC522_T_REALOD_L_REG 0x2D
#define MRFC522_T_COUNTER_VAL_H_REG 0x2E
#define MRFC522_T_CONUTER_VAL_L_REG 0x2F

// page 3
#define MRFC522_RESERVED_30 0x30
#define MRFC522_TEST_SEL_1_REG 0x31
#define MRFC522_TEST_SEL_2_REG 0x32
#define MRFC522_TEST_PIN_EN_REG 0x33
#define MRFC522_TEST_PIN_VALUE_REG 0x34
#define MRFC522_TEST_BUG_REG 0x35
#define MRFC522_AUTO_TEST_REG 0x36
#define MRFC522_VERSION_REG 0x37
#define MRFC522_ANALOG_TEST_REG 0x38
#define MRFC522_TEST_DAC_1_REG 0x39
#define MRFC522_TEST_DAC_2_REG 0x3A
#define MRFC522_TEST_ADC_REG 0x3B
#define MRFC522_RESERVED_3C 0x3C
#define MRFC522_RESERVED_3D 0x3D
#define MRFC522_RESERVED_3E 0x3E
#define MRFC522_RESERVED_3F 0x3F

// PCD
#define PCD_IDLE 0x00
#define PCD_MEM 0x01
#define PCD_GENERATE_RANDOM_ID 0x02
#define PCD_CALC_CRD 0x03
#define PCD_TRANSMIT 0x04
#define PCD_NO_CMD_CHANGE 0x07
#define PCD_RECEIVE 0x08
#define PCD_TRANSCETIVE 0x0C
#define PCD_RESERVED_0D 0x0D
#define PCD_MF_AUTHENT 0x0E
#define PCD_SOFT_RESET 0x0F

// PICC
#define PICC_REQIDL 0x26
#define PICC_ANTICOLL 0x93

// Mifare status
typedef enum { MIFARE_OK = 0, MIFARE_NOTAGERR, MIFARE_ERR } mifare_status_t;

// functions
int mfrc522_init(int32_t baudrate);

mifare_status_t mfrc522_request(uint8_t req_mode);
mifare_status_t mfrc522_to_card(uint8_t cmd,
                                uint8_t *buf,
                                uint8_t len,
                                uint8_t *back_data,
                                uint8_t *back_bits);
mifare_status_t mfrc522_anticoll(uint8_t *back_data, uint8_t *back_bits);

#endif  // MRFC522_H