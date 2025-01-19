cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(pico_sdk_import.cmake)

project(pico_ltr390 C CXX ASM)

pico_sdk_init()

add_subdirectory("src")
add_subdirectory("examples")

# target_include_directories(PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
# target_link_libraries(pico_cyw43_arch_lwip_threadsafe_background pico_stdlib)
