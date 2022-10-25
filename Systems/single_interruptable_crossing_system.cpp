#include <chrono>
#include <cmath>

#include "pico/stdlib.h"

#include "../trafficlight.h"
#include "../trafficlight_group.h"
#include "../controller.h"
#include "../common_sequences.h"

#include "single_interruptable_crossing_system.h"

SingleInterruptableCrossingSystem::SingleInterruptableCrossingSystem(std::vector<std::shared_ptr<TrafficLight>> trafficLights, CrossingStyle crossingStyle, LightType lightType)
{
    setTrafficLights(trafficLights);
    setCrossingStyle(crossingStyle);
    setLightType(lightType);
    setUp();
}

SingleInterruptableCrossingSystem::SingleInterruptableCrossingSystem(std::shared_ptr<TrafficLightGroup> group, CrossingStyle crossingStyle, LightType lightType)
{
    setTrafficLights(group->getTrafficLights());
    setCrossingStyle(crossingStyle);
    setLightType(lightType);
    setUp();
}

void SingleInterruptableCrossingSystem::requestCrossing()
{
    _crossingRequested = true;
}

void SingleInterruptableCrossingSystem::setCrossingStyle(CrossingStyle crossingStyle)
{
    _crossingStyle = crossingStyle;
}

void SingleInterruptableCrossingSystem::setLightType(LightType lightType)
{
    _lightType = lightType;
}

void SingleInterruptableCrossingSystem::setTiming(SingleInterruptableCrossingSystemTimings timing, std::chrono::milliseconds time)
{
    setTimingInternal(timing, time);
}

void SingleInterruptableCrossingSystem::reset()
{
    
}

void SingleInterruptableCrossingSystem::run()
{
    doGreen();
    awaitCrossingRequest();
    doGreenToRed();
    doCrossing();
}

void SingleInterruptableCrossingSystem::setTrafficLights(std::vector<std::shared_ptr<TrafficLight>> trafficLights)
{
    _lightsGroup = std::make_shared<TrafficLightGroup>(trafficLights);
}

void SingleInterruptableCrossingSystem::setUp()
{
    
}

void SingleInterruptableCrossingSystem::doGreen()
{
    auto staticGreenController = std::make_shared<Controller>();
    auto staticGreenSequence = std::make_shared<StaticSequence>(std::chrono::seconds(0), (TrafficLight::Light)(TrafficLight::Light::Green | TrafficLight::Light::RedCrossing));

    staticGreenController->addTrafficLightGroup(_lightsGroup, 0);
    staticGreenController->addSequence(staticGreenSequence, 0);

    staticGreenController->run();
}

void SingleInterruptableCrossingSystem::doGreenToRed()
{
    auto greenToRedController = std::make_shared<Controller>();
    auto staticGreenSequence = std::make_shared<StaticSequence>(getTiming(SingleInterruptableCrossingSystemTimings::DelayAfterCrossingRequest), (TrafficLight::Light)(TrafficLight::Light::Green | TrafficLight::Light::RedCrossing));
    auto greenToRedSequence = std::make_shared<GreenToRedSequence>(getTiming(SingleInterruptableCrossingSystemTimings::DelayBetweenRedLightAndGreenCrossing));

    greenToRedController->addTrafficLightGroup(_lightsGroup, 0);
    greenToRedController->addSequence(staticGreenSequence, 0);
    greenToRedController->addSequence(greenToRedSequence, 0);

    greenToRedController->run();
}

void SingleInterruptableCrossingSystem::doCrossing()
{
    auto crossingTime = getTiming(SingleInterruptableCrossingSystemTimings::CrossingTime);
    auto flashTime = getTiming(SingleInterruptableCrossingSystemTimings::FlashInterval);
    auto numberOfFlashes = (int)ceil((double)crossingTime.count() / (double)(flashTime.count() * 2));

    auto crossingController = std::make_shared<Controller>();
    auto redCrossingToGreenCrossingSequence = std::make_shared<RedCrossingToGreenCrossingSequence>(getTiming(SingleInterruptableCrossingSystemTimings::CrossingTime));
    auto staticOffSequence = std::make_shared<StaticSequence>(getTiming(SingleInterruptableCrossingSystemTimings::OffTimeBetweenGreenAndRedCrossing), TrafficLight::Light::Red);
    auto greenCrossingToRedCrossingSequence = std::make_shared<GreenCrossingToRedCrossingSequence>(getTiming(SingleInterruptableCrossingSystemTimings::DelayBetweenRedCrossingAndGreenLight));
    auto greenCrossingFlash = std::make_shared<FlashingCrossingToGreenSequence>(getTiming(SingleInterruptableCrossingSystemTimings::DelayBetweenCrossingRequests), numberOfFlashes, getTiming(SingleInterruptableCrossingSystemTimings::FlashInterval));
    auto redToGreenSequence = std::make_shared<RedToGreenSequence>(getTiming(SingleInterruptableCrossingSystemTimings::DelayBetweenCrossingRequests), _lightType == LightType::Red_Green ? RedToGreenSequence::SequenceType::Red_Green : RedToGreenSequence::SequenceType::Red_Yellow_Green);

    crossingController->addTrafficLightGroup(_lightsGroup, 0);
    crossingController->addSequence(redCrossingToGreenCrossingSequence, 0);
    
    if (_crossingStyle == CrossingStyle::Standard) {
        crossingController->addSequence(staticOffSequence, 0);
        crossingController->addSequence(greenCrossingToRedCrossingSequence, 0);
        crossingController->addSequence(redToGreenSequence, 0);
    }
    else {
        crossingController->addSequence(greenCrossingFlash, 0);
    }

    _crossingRequested = false;
    crossingController->run();
}

void SingleInterruptableCrossingSystem::awaitCrossingRequest()
{
    while (!_crossingRequested) {
        sleep_ms(100);
    }
}

std::chrono::milliseconds SingleInterruptableCrossingSystem::getStandardTiming(SingleInterruptableCrossingSystemTimings timing) const
{
    switch (timing) {
        case SingleInterruptableCrossingSystemTimings::CrossingTime:
            return std::chrono::seconds(8);
        case SingleInterruptableCrossingSystemTimings::DelayAfterCrossingRequest:
            return std::chrono::seconds(1);
        case SingleInterruptableCrossingSystemTimings::DelayBetweenRedLightAndGreenCrossing:
            return std::chrono::seconds(2);
        case SingleInterruptableCrossingSystemTimings::FlashInterval:
            return std::chrono::milliseconds(500);
        case SingleInterruptableCrossingSystemTimings::OffTimeBetweenGreenAndRedCrossing:
            return std::chrono::seconds(4);
        case SingleInterruptableCrossingSystemTimings::DelayBetweenRedCrossingAndGreenLight:
            return std::chrono::seconds(3);
        case SingleInterruptableCrossingSystemTimings::DelayBetweenCrossingRequests:
            return std::chrono::seconds(5);
    }

    return std::chrono::milliseconds(0);
}
