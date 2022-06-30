add_library(logger "")

target_link_libraries(logger
    pico_stdlib freertos
)

target_sources(logger
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/logger.c
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/logger.h
)

target_include_directories(logger
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)