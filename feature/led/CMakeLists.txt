add_executable(led
        led.c
)

target_link_libraries(led pico_stdlib freertos)
pico_add_extra_outputs(led)
