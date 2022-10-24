#include <memory>
#include <list>
#include <vector>
#include <chrono>

#include "pico/stdlib.h"

#include "../controller.h"
#include "../trafficlight_group.h"
#include "../sequence.h"
#include "../common_sequences.h"

#include "standard_sequence_interruptable_system.h"

StandardSequenceInterruptableSystem::StandardSequenceInterruptableSystem()
{
}

StandardSequenceInterruptableSystem::StandardSequenceInterruptableSystem(LightType lightType, CrossingType crossingType, SequenceType sequenceType) : StandardSequenceInterruptableSystem()
{
    setLightType(lightType);
    setCrossingType(crossingType);
    setSequenceType(sequenceType);
}

StandardSequenceInterruptableSystem::StandardSequenceInterruptableSystem(std::vector<std::shared_ptr<TrafficLight>> trafficLights, SequenceType sequenceType, LightType lightType, CrossingType crossingType) : StandardSequenceInterruptableSystem(lightType, crossingType, sequenceType)
{
    std::vector<std::shared_ptr<TrafficLightGroup>> groups;

    for (auto trafficLight : trafficLights) {
        auto group = std::make_shared<TrafficLightGroup>();
        group->addTrafficLight(trafficLight);

        groups.push_back(group);
    }

    addGroups(groups);
    setUp();
}
    
StandardSequenceInterruptableSystem::StandardSequenceInterruptableSystem(std::vector<std::shared_ptr<TrafficLightGroup>> trafficLightGroups, SequenceType sequenceType, LightType lightType, CrossingType crossingType) : StandardSequenceInterruptableSystem(lightType, crossingType, sequenceType)
{
    addGroups(trafficLightGroups);
    setUp();
}

void StandardSequenceInterruptableSystem::requestCrossing()
{
    _crossingRequested = true;
}

void StandardSequenceInterruptableSystem::requestNextGroup()
{
    _nextGroupRequested = true;
}

void StandardSequenceInterruptableSystem::setLightType(LightType lightType)
{
    _lightType = lightType;
}

void StandardSequenceInterruptableSystem::setCrossingType(CrossingType crossingType)
{
    _crossingType = crossingType;
}

void StandardSequenceInterruptableSystem::setSequenceType(SequenceType sequenceType)
{
    _sequenceType = sequenceType;
}

void StandardSequenceInterruptableSystem::setTiming(Timings timing, std::chrono::milliseconds time)
{
    for (int groupId = 0; groupId < _groups.size(); ++groupId) {
        setTiming(timing, time, groupId);
    }
}

void StandardSequenceInterruptableSystem::setTiming(Timings timing, std::chrono::milliseconds time, unsigned int groupId)
{
    _delays[groupId][timing] = time.count();
}

void StandardSequenceInterruptableSystem::reset()
{
    _currentGroup = 0;
}

void StandardSequenceInterruptableSystem::run()
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

void StandardSequenceInterruptableSystem::addGroups(std::vector<std::shared_ptr<TrafficLightGroup>> groups)
{
    _groups = groups;
}

void StandardSequenceInterruptableSystem::setUp()
{
    setUpAllLightsGroup();
}

void StandardSequenceInterruptableSystem::setUpAllLightsGroup()
{
    _allLightsGroup = std::make_shared<TrafficLightGroup>();

    for (auto group : _groups) {

        auto groupLights = group->getTrafficLights();
        for (auto trafficLight : groupLights) {                
            _allLightsGroup->addTrafficLight(trafficLight);
        }
    }
}

void StandardSequenceInterruptableSystem::awaitNextGroupRequested()
{
    while (_sequenceType == SequenceType::Manual && !_nextGroupRequested) {
        sleep_ms(100);
    }

    sleep_ms(100);
    
    _nextGroupRequested = false;
}

void StandardSequenceInterruptableSystem::doRedToGreen()
{
    auto group = getCurrentGroup();
    auto redToGreenController = std::make_shared<Controller>();
    auto staticRedSequence = std::make_shared<StaticSequence>(getTimingForCurrentGroup(Timings::DelayUntilGreenLight));
    auto redToGreenSequence = std::make_shared<RedToGreenSequence>(getTimingForCurrentGroup(Timings::MinimumTimeUntilRedLight), _lightType == LightType::Red_Green ? RedToGreenSequence::SequenceType::Red_Green : RedToGreenSequence::SequenceType::Red_Yellow_Green);

    redToGreenController->addTrafficLightGroup(_allLightsGroup, 0);
    redToGreenController->addTrafficLightGroup(group, 1);
    redToGreenController->addSequence(staticRedSequence, 0);
    redToGreenController->addSequence(redToGreenSequence, 1);

    redToGreenController->run();
}

void StandardSequenceInterruptableSystem::doGreenToRed()
{
    auto group = getCurrentGroup();
    auto greenToRedController = std::make_shared<Controller>();
    auto greenToRedSequence = std::make_shared<GreenToRedSequence>(std::chrono::milliseconds(0));

    greenToRedController->addTrafficLightGroup(_allLightsGroup, 0);
    greenToRedController->addTrafficLightGroup(group, 1);
    greenToRedController->addSequence(greenToRedSequence, 1);

    greenToRedController->run();
}

void StandardSequenceInterruptableSystem::doCrossingIfRequested()
{
    if (_crossingRequested && _crossingType != CrossingType::None) {
        auto crossingController = std::make_shared<Controller>();
        auto staticRedSequence = std::make_shared<StaticSequence>(getTimingForCurrentGroup(Timings::DelayUntilGreenCrossing));
        auto redCrossingToGreenCrossingSequence = std::make_shared<RedCrossingToGreenCrossingSequence>(getTimingForCurrentGroup(Timings::CrossingTime));
        auto greenCrossingToRedCrossingSequence = std::make_shared<GreenCrossingToRedCrossingSequence>(std::chrono::milliseconds(0));

        crossingController->addTrafficLightGroup(_allLightsGroup, 0);
        crossingController->addSequence(staticRedSequence, 0);
        crossingController->addSequence(redCrossingToGreenCrossingSequence, 0);
        crossingController->addSequence(greenCrossingToRedCrossingSequence, 0);

        _crossingRequested = false;
        crossingController->run();
    }
}

bool StandardSequenceInterruptableSystem::advanceToNextGroup()
{
    if (++_currentGroup < _groups.size()) {
        return true;
    }

    return false;
}

std::chrono::milliseconds StandardSequenceInterruptableSystem::getTimingForGroup(Timings timing, unsigned int groupId) const
{
    auto foundGroup = _delays.find(groupId);
    if (foundGroup != _delays.end()) {
        auto timingsMap = foundGroup->second;
        auto foundTiming = timingsMap.find(timing);
        if (foundTiming != timingsMap.end()) {
            return std::chrono::milliseconds(foundTiming->second);
        }
    }

    return getStandardTiming(timing);
}

std::chrono::milliseconds StandardSequenceInterruptableSystem::getTimingForCurrentGroup(Timings timing) const
{
    return getTimingForGroup(timing, _currentGroup);
}

std::chrono::milliseconds StandardSequenceInterruptableSystem::getStandardTiming(Timings timing) const
{
    switch (timing) {
        case Timings::CrossingTime:
            return std::chrono::seconds(8);
        case Timings::DelayUntilGreenCrossing:
        case Timings::DelayUntilGreenLight:
            return std::chrono::seconds(2);
        case Timings::MinimumTimeUntilRedLight:
            return std::chrono::seconds(10);
    }

    return std::chrono::milliseconds(0);
}

std::shared_ptr<TrafficLightGroup> StandardSequenceInterruptableSystem::getCurrentGroup() const
{
    return _groups[_currentGroup];
}