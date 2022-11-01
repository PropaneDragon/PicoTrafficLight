cmake_minimum_required(VERSION 3.12)

include(pico_sdk_init.cmake)

project(pico_development C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

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