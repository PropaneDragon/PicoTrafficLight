# PicoTrafficLight
![PXL_20221102_152731871](https://user-images.githubusercontent.com/13510828/199532755-37afdb84-506a-4bc0-b0c2-8244ab4d11db.jpg)

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
#### Scale models
Purchase model traffic lights and plug directly into this system for use on scale models.
#### Leaning and safety.
Emulate real systems in a safe environment for kids to learn how to cross safely.
#### Play
Build interactive toys around this system.

## Guide
### Creating a traffic light to control
In the below example we set up a new traffic light given pins for red, yellow, green, red crossing and green crossing lights. We then pass a `TrafficLight::LedType` enum which tells it whether it's a common anode or common cathode configuration 
`std::make_shared<TrafficLight>(0u, 1u, 2u, 3u, 4u, TrafficLight::LedType::CommonAnode);`

You can also just pass red, yellow and green pins without crossing pins to set up for a traffic light system without a crossin, or you can pass only red and green crossing pins to set up for a stand-alone crossing light system.
If you want, you can manipulate this traffic light manually using the methods provided in the `TrafficLight` class, such as 

`void turnLightsOn(Light lights)`

Where `Light` is an enum of the lights to switch on. Multiple lights can be switched on at once by utilising the binary OR operator. Example: `void turnLightsOn(TrafficLight::Light::Red | TrafficLight::Light::Yellow | TrafficLight::Light::RedCrossing)` which will turn on the red, yellow and red crossing lights all at once. 

## How to build
#### Easy method
1. Fork this repository.
1. Modify the main.cpp file to behave as you want.
1. Run the build action in GitHub.
1. Download the produced trafficlight.uf2 file in the Binary artifact.
1. Plug your Pico into your machine, copy the trafficlight.uf2 file onto it.
1. Enjoy.

If programming isn't your thing, don't worry. I'm still working on a solution to build a custom system using alternate methods.
