cmake_minimum_required(VERSION 3.17)

# Set project data
set(PROJECT_NAME "freertos-pico")
set(VERSION_NUMBER "1.0.0")
set(BUILD_NUMBER "1")

# Make project data accessible to compiler
add_compile_definitions(APP_NAME="${PROJECT_NAME}")
add_compile_definitions(APP_VERSION="${VERSION_NUMBER}")
add_compile_definitions(BUILD_NUM=${BUILD_NUMBER})

# Enable fetch pico sdk from git
set(ENV{PICO_SDK_FETCH_FROM_GIT} 1)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(freertos-pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_subdirectory(freertos)

SET(FEATURE "ALL" CACHE STRING "Choise FEATURE which you want to use")

message("Feature flag `${FEATURE}`")
if ((FEATURE STREQUAL "ALL") OR (FEATURE STREQUAL "led"))
    message("Feature led on")
    add_subdirectory(src/feature/led)
endif()
if ((FEATURE STREQUAL "ALL") OR (FEATURE STREQUAL "dummy"))
    message("Feature dummy on")
    add_subdirectory(src/feature/dummy)
endif()
unset(FEATURE CACHE)