#ifndef MRFC522_H
#define MRFC522_H

// PICC
#define PICC_REQIDL 0x26

// MI
#define MI_OK 0x00
#define MI_NOTAGERR 0x01
#define MI_ERR 0x02

// PCD
#define PCD_IDLE 0x0
#define PCD_TRANSCETIVE 0x0C
#define PCD_AUTHENT 0x0E
#define PCD_RESETPHASE 0x0F

// FIFO
#define FIFO_DATA_REG 0x09
#define FIFO_LEVEL_REG 0x0A

// Control
#define CONTROL_REG 0x0C
#define COMMAND_REG 0x01

// ERROR
#define ERROR_REG 0x06

#define T_MODE_REG 0x2A
#define T_PRESCALER_REG 0x2B
#define T_RELOAD_REG_L 0x2D
#define T_RELOAD_REG_H 0x2C
#define TX_AUTO_REG 0x15
#define MODE_REG 0x11
#define TX_CONTROL_REG 0x14
#define BIT_FRAMING_REG 0x0D


#endif  // MRFC522_H