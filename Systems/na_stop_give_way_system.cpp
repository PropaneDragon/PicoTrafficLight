#include <cmath>

#include "../controller.h"
#include "../common_sequences.h"
#include "../trafficlight.h"
#include "../trafficlight_group.h"

#include "na_stop_give_way_system.h"

NAStopGiveWaySystem::NAStopGiveWaySystem(std::vector<std::shared_ptr<TrafficLight>> trafficLights, unsigned int priorityLightId)
{
    auto priorityGroup = std::make_shared<TrafficLightGroup>();
    auto stopGroup = std::make_shared<TrafficLightGroup>();

    if (priorityLightId >= trafficLights.size()) {
        priorityLightId = 0;
    }

    for (size_t offset = 0; offset < trafficLights.size(); ++offset) {
        auto trafficLight = trafficLights[offset];

        if (offset == priorityLightId) {
            priorityGroup->addTrafficLight(trafficLight);
        }
        else {
            stopGroup->addTrafficLight(trafficLight);
        }
    }

    setGroups(priorityGroup, stopGroup);
}

NAStopGiveWaySystem::NAStopGiveWaySystem(std::shared_ptr<TrafficLightGroup> priorityTrafficLightGroup, std::shared_ptr<TrafficLightGroup> stopLightGroup)
{
    setGroups(priorityTrafficLightGroup, stopLightGroup);
}

void NAStopGiveWaySystem::run()
{
    flash();
}

void NAStopGiveWaySystem::setTiming(NAStopGiveWaySystemTimings timing, std::chrono::milliseconds time)
{
    setTimingInternal(timing, time);
}

void NAStopGiveWaySystem::setGroups(std::shared_ptr<TrafficLightGroup> priority, std::shared_ptr<TrafficLightGroup> stop)
{
    _priorityGroup = priority;
    _stopGroup = stop;
}

void NAStopGiveWaySystem::flash()
{    
    auto flashInterval = getTiming(NAStopGiveWaySystemTimings::FlashInterval);
    auto loopLength = getTiming(NAStopGiveWaySystemTimings::LoopTime);
    auto flashesPerRun = (int)ceil((double)loopLength.count() / (double)(flashInterval.count() * 2));

    auto flashController = std::make_shared<Controller>();
    
    auto staticRedSequence = std::make_shared<StaticSequence>(std::chrono::milliseconds(0), (TrafficLight::Light)(TrafficLight::Light::Red | TrafficLight::Light::RedCrossing));
    auto staticYellowSequence = std::make_shared<StaticSequence>(getTiming(NAStopGiveWaySystemTimings::FlashInterval), (TrafficLight::Light)(TrafficLight::Light::Yellow | TrafficLight::Light::RedCrossing));
    auto staticOffSequenceRed = std::make_shared<StaticSequence>(std::chrono::milliseconds(0), TrafficLight::Light::RedCrossing);
    auto staticOffSequenceYellow = std::make_shared<StaticSequence>(getTiming(NAStopGiveWaySystemTimings::FlashInterval), TrafficLight::Light::RedCrossing);
    
    flashController->addTrafficLightGroup(_priorityGroup, 0);
    flashController->addTrafficLightGroup(_stopGroup, 1);

    flashController->addSequence(staticRedSequence, 1);
    flashController->addSequence(staticYellowSequence, 0);
    flashController->addSequence(staticOffSequenceRed, 1);
    flashController->addSequence(staticOffSequenceYellow, 0);

    for (auto flash = flashesPerRun; flash > 0; --flash) {
        flashController->run();
    }
}

std::chrono::milliseconds NAStopGiveWaySystem::getStandardTiming(NAStopGiveWaySystemTimings timing) const
{
    switch (timing) {
        case NAStopGiveWaySystemTimings::FlashInterval:
            return std::chrono::seconds(1);
        case NAStopGiveWaySystemTimings::LoopTime:
            return std::chrono::seconds(10);
    }

    return std::chrono::milliseconds(0);
}