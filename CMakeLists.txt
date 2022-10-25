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
        common_sequences.h

        Systems/abstract_system.h
        Systems/light_test_system.h
        Systems/light_test_system.cpp
        Systems/sequenced_interruptable_system.h
        Systems/sequenced_interruptable_system.cpp
        Systems/single_interruptable_crossing_system.h
        Systems/single_interruptable_crossing_system.cpp
        Systems/na_stop_give_way_system.h
        Systems/na_stop_give_way_system.cpp
)

target_link_libraries(trafficlight pico_stdlib)
target_link_libraries(trafficlight pico_multicore)

pico_enable_stdio_usb(trafficlight 1)
pico_enable_stdio_uart(trafficlight 1)

pico_add_extra_outputs(trafficlight)