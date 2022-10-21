add_executable(trafficlight
        main.cpp
        controller.h
        controller.cpp
        trafficlight.h
        trafficlight.cpp
        trafficlight_group.h
        trafficlight_group.cpp
        sequence.h
        sequence.cpp
        )

target_link_libraries(trafficlight pico_stdlib)
target_link_libraries(trafficlight pico_multicore)

pico_enable_stdio_usb(trafficlight 1)
pico_enable_stdio_uart(trafficlight 1)

pico_add_extra_outputs(trafficlight)