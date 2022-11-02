# PicoTrafficLight
![Main image](https://user-images.githubusercontent.com/13510828/199532755-37afdb84-506a-4bc0-b0c2-8244ab4d11db.jpg)

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

## Examples
<img src="/.github/images/flashing-yellow-crossing.gif" height="200" /> <img src="/.github/images/american-style.gif" height="200" /> <img src="/.github/images/individually-sequenced-crossing.gif" height="200" /> <img src="/.github/images/standard-crossing.gif" height="200" />

## Use cases
#### Scale models
Purchase model traffic lights and plug directly into this system for use on scale models.
#### Leaning and safety.
Emulate real systems in a safe environment for kids to learn how to cross safely.
#### Play
Build interactive toys around this system.

## Guide
### Creating a traffic light to control
In the below example we set up a new traffic light given pins for red, yellow, green, red crossing and green crossing lights. We then pass a `TrafficLight::LedType` enum which tells it whether it's a common anode or common cathode configuration:

`std::make_shared<TrafficLight>(0u, 1u, 2u, 3u, 4u, TrafficLight::LedType::CommonAnode);`

You can also just pass red, yellow and green pins without crossing pins to set up for a traffic light system without a crossing, or you can pass only red and green crossing pins to set up for a stand-alone crossing light system.
If you want, you can manipulate this traffic light manually using the methods provided in the `TrafficLight` class, such as 

`void turnLightsOn(Light lights)`

Where `Light` is an enum of the lights to switch on. Multiple lights can be switched on at once by utilising the binary OR operator (| not ||). Example: `void turnLightsOn(TrafficLight::Light::Red | TrafficLight::Light::Yellow | TrafficLight::Light::RedCrossing)` which will turn on the red, yellow and red crossing lights all at once. See more available methods in [trafficlight.h](/trafficlight.h).

### Creating traffic light groups
In addition to this, you can create groups of traffic lights which enable you to manipulate a set of traffic lights from one location. To create a group, first create your traffic lights as above, add them to a `std::vector<std::shared_ptr<TrafficLight>>` then pass them into a group as shown below:

```
std::vector<std::shared_ptr<TrafficLight>> trafficLights = { trafficLight1, trafficLight2 };
auto group = std::make_shared<TrafficLightGroup>(trafficLights);
```

You can then manipulate a group in a similar way to a stand-alone `TrafficLight`, but any calls to methods on a group will affect all `TrafficLight`s within it at once. See [trafficlight_group.h](trafficlight_group.h) for available methods.

### Controllers and sequences
If manually manipulating individual lights isn't your thing you can create or utilise `Controller`s and `Sequence`s to control groups of traffic lights automatically. A `Controller` is a simple system that allows you to simply run through a list of sequences and apply them to different traffic light groups. See the below example:

```
std::vector<std::shared_ptr<TrafficLight>> trafficLights = { trafficLight1, trafficLight2 };

auto group = std::make_shared<TrafficLightGroup>(trafficLights);
auto controller = std::make_shared<Controller>();
auto sequence = std::make_shared<TestSequence>(std::chrono::milliseconds(300));

controller->addTrafficLightGroup(group, 0);
controller->addSequence(sequence, 0);

controller->run();
```

In the example above, a controller and sequence is created. The sequence is pre-made and available in [common_sequences](/common_sequences.h) and runs through all the available lights in a pattern to test they're working. You can utilise any sequence you like. The controller is then passed a traffic light group, which is given the ID of 0, and the sequence is added to manipulate the group ID of 0. When `run()` is called on the controller it runs through the sequences passed once then finishes. Let's expand it a bit:

```
std::vector<std::shared_ptr<TrafficLight>> allTrafficLights = { trafficLight1, trafficLight2 };
std::vector<std::shared_ptr<TrafficLight>> trafficLightSet1 = { trafficLight1 };
std::vector<std::shared_ptr<TrafficLight>> trafficLightSet2 = { trafficLight2 };

auto allTrafficLightsGroup = std::make_shared<TrafficLightGroup>(trafficLights);
auto trafficLightGroup1 = std::make_shared<TrafficLightGroup>(trafficLights);
auto trafficLightGroup2 = std::make_shared<TrafficLightGroup>(trafficLights);

auto controller = std::make_shared<Controller>();
auto sequence = std::make_shared<TestSequence>(std::chrono::milliseconds(300));

controller->addTrafficLightGroup(allTrafficLightsGroup, 0);
controller->addTrafficLightGroup(trafficLightGroup1, 1);
controller->addTrafficLightGroup(trafficLightGroup2, 2);

controller->addSequence(sequence, 0);
controller->addSequence(sequence, 1);
controller->addSequence(sequence, 2);

controller->run();
```

In this example an additional two groups have been created which just consist of the first traffic light and second traffic light respectively. These groups are given the ID 1 and 2, and the same `TestSequence` sequence is added to those groups. When run, this will start the test sequence on group 0, which is both the traffic lights, so they will run through the same sequence together. Once tht is complete it will move onto the next sequence at ID 1, which is `trafficLight1` in the example. `trafficLight2` will be ignored and remain on the last active light, and `trafficLight1` will loop through the sequence. Finally, `trafficLight2` will loop through its sequence while `trafficLight1` remains static.

You don't have to only use a single controller, you can link controllers together and run them in seqeunce by simply calling `run()` on a sequence after a previous one has completed. For examples of this in use, see [Systems/sequenced_interruptable_system.cpp](/Systems/sequenced_interruptable_system.cpp).

## How to build
#### Easy method
1. Fork this repository.
1. Modify the main.cpp file to behave as you want.
1. Run the build action in GitHub.
1. Download the produced trafficlight.uf2 file in the Binary artifact.
1. Plug your Pico into your machine, copy the trafficlight.uf2 file onto it.
1. Enjoy.

If programming isn't your thing, don't worry. I'm still working on a solution to build a set of standard traffic light setups and allowing you to create a custom system easily, but this may take some time.
