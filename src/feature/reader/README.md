# Reader 

## Layout

| RC522 PIN | pin      | Description     |
| --------  | -------  | --------------- |
| 3.3v      | 3.3V     | 3.3V PWR        |
| RST       | pin 1    | GP0 SPI0 RX     |
| GND       | GND      | GND             |
| MISO      | pin 6    | GP4 SPIO RX     |
| MOSI      | pin 5    | GP3 SPIO TX     |
| SCK       | pin 4    | GP2 SPIO SCLK   |
| SDA       | pin 2    | GP1 SPIO CSn    |

## Reference

[Raspberry Pi Pico筆記(13)：RFID讀取並控制LED燈號](https://atceiling.blogspot.com/2021/04/raspberry-pi-pico13rfidled.html)  
[RC522 RFID Reader Module with Raspberry Pi Pico](https://microcontrollerslab.com/raspberry-pi-pico-rfid-rc522-micropython/#Installing_MicroPython_MFRC522_Library)  
[miguelbalboa/rfid](https://github.com/miguelbalboa/rfid)  
[pimylifeup/MFRC522-python](https://github.com/pimylifeup/MFRC522-python)  
[Tasm-Devil/micropython-mfrc522-esp32](https://github.com/Tasm-Devil/micropython-mfrc522-esp32)  
[janfietz/mfrc522](https://github.com/janfietz/mfrc522)  
[MFRC522 Standard performance MIFARE and NTAG frontend](https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf)  