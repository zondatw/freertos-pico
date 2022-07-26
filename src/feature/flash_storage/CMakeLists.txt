add_executable(flash_storage
    flash_storage.c
)

target_link_libraries(flash_storage
    PUBLIC
        pico_stdlib freertos
    PRIVATE
        logger
)

# enable usb output, disable uart output
pico_enable_stdio_usb(flash_storage 1)
pico_enable_stdio_uart(flash_storage 1)

pico_add_extra_outputs(flash_storage)

# install

install (FILES ${CMAKE_BINARY_DIR}/src/feature/flash_storage/flash_storage.uf2
    DESTINATION ./
)