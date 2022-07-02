add_executable(msg_queue
    msg_queue.c
)

target_link_libraries(msg_queue
    PUBLIC
        pico_stdlib freertos
    PRIVATE
        logger
)

# enable usb output, disable uart output
pico_enable_stdio_usb(msg_queue 1)
pico_enable_stdio_uart(msg_queue 1)

pico_add_extra_outputs(msg_queue)

# install

install (FILES ${CMAKE_BINARY_DIR}/src/feature/msg_queue/msg_queue.uf2
    DESTINATION ./
)