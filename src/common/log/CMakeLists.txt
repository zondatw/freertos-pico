add_library(log "")

target_link_libraries(log
    pico_stdlib freertos
)

target_sources(log
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/log.c
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/log.h
)

target_include_directories(log
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)