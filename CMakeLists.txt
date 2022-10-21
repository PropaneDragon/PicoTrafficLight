add_executable(trafficlight
        trafficlight.cpp
        )

target_link_libraries(trafficlight pico_stdlib)
target_link_libraries(trafficlight pico_multicore)

pico_enable_stdio_usb(trafficlight 1)
pico_enable_stdio_uart(trafficlight 1)

pico_add_extra_outputs(trafficlight)