#include <memory>
#include <list>
#include <vector>
#include <chrono>

#include "pico/stdlib.h"

#include "../controller.h"
#include "../trafficlight_group.h"
#include "../sequence.h"
#include "../common_sequences.h"

#include "sequenced_interruptable_system.h"

SequencedInterruptableSystem::SequencedInterruptableSystem()
{
}

SequencedInterruptableSystem::SequencedInterruptableSystem(LightType lightType, CrossingType crossingType, SequenceType sequenceType) : SequencedInterruptableSystem()
{
    setLightType(lightType);
    setCrossingType(crossingType);
    setSequenceType(sequenceType);
}

SequencedInterruptableSystem::SequencedInterruptableSystem(std::vector<std::shared_ptr<TrafficLight>> trafficLights, SequenceType sequenceType, LightType lightType, CrossingType crossingType) : SequencedInterruptableSystem(lightType, crossingType, sequenceType)
{
    std::vector<std::shared_ptr<TrafficLightGroup>> groups;

    for (auto trafficLight : trafficLights) {
        auto group = std::make_shared<TrafficLightGroup>();
        group->addTrafficLight(trafficLight);

        groups.push_back(group);
    }

    setGroups(groups);
    setUp();
}
    
SequencedInterruptableSystem::SequencedInterruptableSystem(std::vector<std::shared_ptr<TrafficLightGroup>> trafficLightGroups, SequenceType sequenceType, LightType lightType, CrossingType crossingType) : SequencedInterruptableSystem(lightType, crossingType, sequenceType)
{
    setGroups(trafficLightGroups);
    setUp();
}

void SequencedInterruptableSystem::requestCrossing()
{
    _crossingRequested = true;
}

void SequencedInterruptableSystem::requestNextGroup()
{
    _nextGroupRequested = true;
}

void SequencedInterruptableSystem::setLightType(LightType lightType)
{
    _lightType = lightType;
}

void SequencedInterruptableSystem::setCrossingType(CrossingType crossingType)
{
    _crossingType = crossingType;
}

void SequencedInterruptableSystem::setSequenceType(SequenceType sequenceType)
{
    _sequenceType = sequenceType;
}

void SequencedInterruptableSystem::setTiming(SequencedInterruptableSystemTimings timing, std::chrono::milliseconds time)
{    
    setTimingInternal(timing, time);
}

void SequencedInterruptableSystem::setTiming(SequencedInterruptableSystemTimings timing, std::chrono::milliseconds time, unsigned int groupId)
{
    setTimingInternal(timing, time, groupId);
}

void SequencedInterruptableSystem::reset()
{
    _currentGroup = 0;
}

void SequencedInterruptableSystem::run()
{
    reset();

    do {
        doRedToGreen();
        awaitNextGroupRequested();
        doGreenToRed();
        doCrossingIfRequested();            
    }
    while (advanceToNextGroup());
}

void SequencedInterruptableSystem::setGroups(std::vector<std::shared_ptr<TrafficLightGroup>> groups)
{
    _groups = groups;
}

void SequencedInterruptableSystem::setUp()
{
    setUpAllLightsGroup();
}

void SequencedInterruptableSystem::setUpAllLightsGroup()
{
    _allLightsGroup = std::make_shared<TrafficLightGroup>();

    for (auto group : _groups) {

        auto groupLights = group->getTrafficLights();
        for (auto trafficLight : groupLights) {                
            _allLightsGroup->addTrafficLight(trafficLight);
        }
    }
}

void SequencedInterruptableSystem::awaitNextGroupRequested()
{
    while (_sequenceType == SequenceType::Manual && !_nextGroupRequested) {
        sleep_ms(100);
    }

    sleep_ms(100);
    
    _nextGroupRequested = false;
}

void SequencedInterruptableSystem::doRedToGreen()
{
    auto group = getCurrentGroup();
    auto redToGreenController = std::make_shared<Controller>();
    auto staticRedSequence = std::make_shared<StaticSequence>(getTimingForCurrentGroup(SequencedInterruptableSystemTimings::DelayUntilGreenLight));
    auto redToGreenSequence = std::make_shared<RedToGreenSequence>(getTimingForCurrentGroup(SequencedInterruptableSystemTimings::MinimumTimeUntilRedLight), _lightType == LightType::Red_Green ? RedToGreenSequence::SequenceType::Red_Green : RedToGreenSequence::SequenceType::Red_Yellow_Green);

    redToGreenController->addTrafficLightGroup(_allLightsGroup, 0);
    redToGreenController->addTrafficLightGroup(group, 1);
    redToGreenController->addSequence(staticRedSequence, 0);
    redToGreenController->addSequence(redToGreenSequence, 1);

    redToGreenController->run();
}

void SequencedInterruptableSystem::doGreenToRed()
{
    auto group = getCurrentGroup();
    auto greenToRedController = std::make_shared<Controller>();
    auto greenToRedSequence = std::make_shared<GreenToRedSequence>(std::chrono::milliseconds(0));

    greenToRedController->addTrafficLightGroup(_allLightsGroup, 0);
    greenToRedController->addTrafficLightGroup(group, 1);
    greenToRedController->addSequence(greenToRedSequence, 1);

    greenToRedController->run();
}

void SequencedInterruptableSystem::doCrossingIfRequested()
{
    if (_crossingRequested && _crossingType != CrossingType::None) {
        auto crossingController = std::make_shared<Controller>();
        auto staticRedSequence = std::make_shared<StaticSequence>(getTimingForCurrentGroup(SequencedInterruptableSystemTimings::DelayUntilGreenCrossing));
        auto redCrossingToGreenCrossingSequence = std::make_shared<RedCrossingToGreenCrossingSequence>(getTimingForCurrentGroup(SequencedInterruptableSystemTimings::CrossingTime));
        auto greenCrossingToRedCrossingSequence = std::make_shared<GreenCrossingToRedCrossingSequence>(std::chrono::milliseconds(0));

        crossingController->addTrafficLightGroup(_allLightsGroup, 0);
        crossingController->addSequence(staticRedSequence, 0);
        crossingController->addSequence(redCrossingToGreenCrossingSequence, 0);
        crossingController->addSequence(greenCrossingToRedCrossingSequence, 0);

        _crossingRequested = false;
        crossingController->run();
    }
}

bool SequencedInterruptableSystem::advanceToNextGroup()
{
    if (++_currentGroup < _groups.size()) {
        return true;
    }

    return false;
}

std::chrono::milliseconds SequencedInterruptableSystem::getTimingForCurrentGroup(SequencedInterruptableSystemTimings timing) const
{
    return getTiming(timing, _currentGroup);
}

std::chrono::milliseconds SequencedInterruptableSystem::getStandardTiming(SequencedInterruptableSystemTimings timing) const
{
    switch (timing) {
        case SequencedInterruptableSystemTimings::CrossingTime:
            return std::chrono::seconds(8);
        case SequencedInterruptableSystemTimings::DelayUntilGreenCrossing:
        case SequencedInterruptableSystemTimings::DelayUntilGreenLight:
            return std::chrono::seconds(2);
        case SequencedInterruptableSystemTimings::MinimumTimeUntilRedLight:
            return std::chrono::seconds(10);
    }

    return std::chrono::milliseconds(0);
}

std::shared_ptr<TrafficLightGroup> SequencedInterruptableSystem::getCurrentGroup() const
{
    return _groups[_currentGroup];
}