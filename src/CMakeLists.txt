add_library(sensor_ltr390 INTERFACE)

target_include_directories(sensor_ltr390 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_sources(sensor_ltr390 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ltr390.c
)

target_link_libraries(sensor_ltr390 INTERFACE
    pico_stdlib
    hardware_i2c
)
