add_executable(reader
    reader.c
    mfrc522.c
)

target_link_libraries(reader
    PUBLIC
        pico_stdlib freertos hardware_spi
    PRIVATE
        logger
)

# enable usb output, disable uart output
pico_enable_stdio_usb(reader 1)
pico_enable_stdio_uart(reader 1)

pico_add_extra_outputs(reader)

# install

install (FILES ${CMAKE_BINARY_DIR}/src/feature/reader/reader.uf2
    DESTINATION ./
)