add_executable(dummy
    dummy.c
)

target_link_libraries(dummy pico_stdlib freertos)

# enable usb output, disable uart output
pico_enable_stdio_usb(dummy 1)
pico_enable_stdio_uart(dummy 0)

pico_add_extra_outputs(dummy)

# install

install (FILES ${CMAKE_BINARY_DIR}/src/feature/dummy/dummy.uf2
    DESTINATION ./
)       