set(FREERTOS_SOURCE FreeRTOS-Kernel)

add_library(freertos
    ${FREERTOS_SOURCE}/croutine.c
    ${FREERTOS_SOURCE}/event_groups.c
    ${FREERTOS_SOURCE}/list.c
    ${FREERTOS_SOURCE}/queue.c
    ${FREERTOS_SOURCE}/stream_buffer.c
    ${FREERTOS_SOURCE}/tasks.c
    ${FREERTOS_SOURCE}/timers.c
    ${FREERTOS_SOURCE}/portable/MemMang/heap_3.c
    ${FREERTOS_SOURCE}/portable/GCC/ARM_CM0/port.c
)

target_include_directories(freertos PUBLIC
    .
    ${FREERTOS_SOURCE}/include
    ${FREERTOS_SOURCE}/portable/GCC/ARM_CM0
)
