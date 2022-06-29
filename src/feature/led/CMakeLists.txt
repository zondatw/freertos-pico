add_executable(led
        led.c
)

target_link_libraries(led pico_stdlib freertos)

# enable usb output, disable uart output
pico_enable_stdio_usb(led 1)
pico_enable_stdio_uart(led 1)

pico_add_extra_outputs(led)

# install

install (FILES ${CMAKE_BINARY_DIR}/src/feature/led/led.uf2
        DESTINATION ./
)