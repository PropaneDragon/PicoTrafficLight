# PicoTrafficLight
An easy to use traffic light controller for Raspberry Pico and possibly other RP2040 boards. Load this on to your board and attach to your projects to have a fully functional traffic light system that emulates systems from all over the world.

## Features
- Supports both common anode and common cathode light configurations on a light by light basis.
- Contains common sequence configurations such as red-green or red-red+yellow-green with simple arguments.
- Supports crossing lights as well as interrupts for button presses.
- Supports manual advancing of lights from a custom trigger so you can set them up how you like them.
- Contains a number of built systems to quickly get up and running.
  - LightTestSystem - Runs through all LEDs in your traffic light to allow you to check which ones are operational.
  - NAStopGiveWaySystem - Acts like flashing North American traffic lights where one direction flashes red and another flashes yellow.
  - SequencedInterruptableSystem - Takes groups of traffic lights and sequences them one after another. Supports requests for crossing lights.
  - SingleInterruptableCrossingSystem - Emulates traffic lights on a crossing where it will stay green until a crossing is requested and change to allow pedestrians to cross. Configurable to flash or change normally.
- Configurable groups allows for sequencing large sets of lights.
- Customisable sequences if the existing configurations aren't quite right for your use case.

## Use cases
### Scale models
Purchase model traffic lights and plug directly into this system for use on scale models.
### Leaning and safety.
Emulate real systems in a safe environment for kids to learn how to cross safely.
### Play
Build interactive toys around this system.

## How to build
- Coming soon
