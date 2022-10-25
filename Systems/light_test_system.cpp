#include "../trafficlight.h"
#include "../trafficlight_group.h"
#include "../controller.h"
#include "../common_sequences.h"

#include "light_test_system.h"

void LightTestSystem::setTiming(LightTestSystemTimings timing, std::chrono::milliseconds delay)
{
    setTimingInternal(timing, delay);
}

void LightTestSystem::run()
{
    auto testController = std::make_shared<Controller>();
    auto trafficLightGroup = std::make_shared<TrafficLightGroup>(_trafficLights);
    auto testSequence = std::make_shared<TestSequence>(getTiming(LightTestSystemTimings::AnimationDelay));

    testController->addTrafficLightGroup(trafficLightGroup, 0);
    testController->addSequence(testSequence, 0);

    testController->run();
}

std::chrono::milliseconds LightTestSystem::getStandardTiming(LightTestSystemTimings timing) const
{
    return std::chrono::milliseconds(150);
}