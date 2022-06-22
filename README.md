# FreeRTOS for raspberry pico

## Build

```shell
mkdir build
cd build
cmake ..
make
```

## Setup Pico

hold `BOOTSEL` and tap `RESET`, cp build/feature/led/led.uf2 file to pico's folder

## Init project process

### copy pico-sdk

copy from https://github.com/raspberrypi/pico-examples/blob/master/pico_sdk_import.cmake


### init submoudle

```shell
git submodule add https://github.com/FreeRTOS/FreeRTOS-Kernel freertos/FreeRTOS-Kernel
```

### Porting

copy from FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c

because RP2040 is Arm Cortex-M0+ processor.

> Designed by Raspberry Pi, RP2040 features a dual-core Arm Cortex-M0+ processor with 264KB internal RAM and support for up to 16MB of off-chip Flash. A wide range of flexible I/O options includes I2C, SPI, and — uniquely — Programmable I/O (PIO). These support endless possible applications for this small and affordable package.

### FreeRTOSConfig.h

copy from [official Customisation](https://www.freertos.org/a00110.html)

## Reference

[How To Use FreeRTOS on the Raspberry Pi Pico (RP2040) Part 1: VSCode Setup and Blinky Test!](https://www.youtube.com/watch?v=jCZxStjzGA8)
[How to use FreeRTOS with the Raspberry Pi Pico](https://blog.smittytone.net/2022/02/24/how-to-use-freertos-with-the-raspberry-pi-pico/)