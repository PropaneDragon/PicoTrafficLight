#include <chrono>
#include <memory>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "Systems/sequenced_interruptable_system.h"
#include "Systems/single_interruptable_crossing_system.h"
#include "Systems/na_stop_give_way_system.h"

#include "controller.h"
#include "trafficlight.h"
#include "trafficlight_group.h"
#include "sequence.h"
#include "common_sequences.h"

std::shared_ptr<SequencedInterruptableSystem> _standardSystem;
std::shared_ptr<SingleInterruptableCrossingSystem> _flashingCrossingSystem, _standardCrossingSystem;
std::shared_ptr<NAStopGiveWaySystem> _stopGiveWaySystem;

void runSequencedSystem(std::vector<std::shared_ptr<TrafficLight>> trafficLights)
{
    if (!_standardSystem) {
        _standardSystem = std::make_shared<SequencedInterruptableSystem>(trafficLights, SequencedInterruptableSystem::SequenceType::Auto);
        _standardSystem->setTiming(SequencedInterruptableSystemTimings::MinimumTimeUntilRedLight, std::chrono::seconds(6), 0);
        _standardSystem->setTiming(SequencedInterruptableSystemTimings::MinimumTimeUntilRedLight, std::chrono::seconds(2), 1);
        _standardSystem->setTiming(SequencedInterruptableSystemTimings::CrossingTime, std::chrono::seconds(6), 0);
        _standardSystem->setTiming(SequencedInterruptableSystemTimings::CrossingTime, std::chrono::seconds(3), 1);
    }

    _standardSystem->requestCrossing();
    _standardSystem->run();
}

void runFlashingCrossingSystem(std::vector<std::shared_ptr<TrafficLight>> trafficLights)
{
    if (!_flashingCrossingSystem) {
        _flashingCrossingSystem = std::make_shared<SingleInterruptableCrossingSystem>(trafficLights, SingleInterruptableCrossingSystem::CrossingStyle::Flashing);
    }

    _flashingCrossingSystem->requestCrossing();
    _flashingCrossingSystem->run();
}

void runStandardCrossingSystem(std::vector<std::shared_ptr<TrafficLight>> trafficLights)
{
    if (!_standardCrossingSystem) {
        _standardCrossingSystem = std::make_shared<SingleInterruptableCrossingSystem>(trafficLights, SingleInterruptableCrossingSystem::CrossingStyle::Standard);
    }

    _standardCrossingSystem->requestCrossing();
    _standardCrossingSystem->run();
}

void runNAStopGiveWaySystem(std::vector<std::shared_ptr<TrafficLight>> trafficLights)
{
    if (!_stopGiveWaySystem) {
        _stopGiveWaySystem = std::make_shared<NAStopGiveWaySystem>(trafficLights, 1u);
    }

    _stopGiveWaySystem->run();
}

void lightsThread()
{
    auto northTrafficLight = std::make_shared<TrafficLight>(0u, 1u, 2u, 3u, 4u, TrafficLight::LedType::CommonAnode);
    auto southTrafficLight = std::make_shared<TrafficLight>(5u, 6u, 7u, 8u, 9u, TrafficLight::LedType::CommonAnode);
    
    std::vector<std::shared_ptr<TrafficLight>> trafficLights = { northTrafficLight, southTrafficLight };

    while(true) {
        for (auto loops = 2; loops > 0; --loops) {
            runSequencedSystem(trafficLights);
        }
        runFlashingCrossingSystem(trafficLights);
        runStandardCrossingSystem(trafficLights);
        runNAStopGiveWaySystem(trafficLights);
    }
}

void initPin(uint pinId, int direction = GPIO_OUT) {
    gpio_init(pinId);
    gpio_set_dir(pinId, GPIO_OUT);
}

void inputsThread()
{
    initPin(13, GPIO_IN);
    initPin(PICO_DEFAULT_LED_PIN);
    
    while (true) {   
        if (gpio_get(13)) {
            if (_standardSystem) {
                _standardSystem->requestCrossing();
                //_standardSystem->requestNextGroup();
            }

            sleep_ms(1000);
        }
        
        sleep_ms(100);
    }
}

int main() 
{
    multicore_launch_core1(&inputsThread);
    lightsThread();
}