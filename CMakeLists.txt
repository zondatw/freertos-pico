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

add_subdirectory(freertos)
add_subdirectory(src)