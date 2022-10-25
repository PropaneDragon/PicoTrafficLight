#pragma once

#include <memory>
#include <list>
#include <vector>
#include <map>

#include "abstract_system.h"

class TrafficLight;
class TrafficLightGroup;
class Sequence;

enum class SequencedInterruptableSystemTimings
{
    DelayUntilGreenLight, //Delay between red light of previous group and starting green light sequence of the current group.
    MinimumTimeUntilRedLight, //The time to wait until it is acceptable to switch back to red again.
    DelayUntilGreenCrossing, //Time to wait between red vehicle light and green crossing light.
    CrossingTime, //Allocated time to allow for crossing.
    OffTimeBetweenGreenAndRedCrossing, //Time where both red and green crossing lights are off before switching back to red.
};

/// @brief A simple system that can take n number of traffic lights or groups and sequence them one after another
/// supporting interrupts for crossing buttons and manual advancing of light stages.
class SequencedInterruptableSystem : public AbstractSystem<SequencedInterruptableSystemTimings>
{
public:
    enum class LightType { Red_Yellow_Green, Red_Green };
    enum class CrossingType { None, Standard };
    enum class SequenceType { Auto, Manual };

    /// @brief Constructs a sequenced system from individual traffic lights. Each passed in TrafficLight gets its own
    /// individual group.
    /// @param trafficLights A vector of individual traffic lights to sequence together.
    /// @param sequenceType The type of sequence. Auto allows the system to advance sequences on its own, and manual allows
    /// for advancing of sequences through requestNextGroup().
    /// @param lightType The type of lighting sequence to use between red and green stages.
    /// @param crossingType The type of crossing present.
    SequencedInterruptableSystem(std::vector<std::shared_ptr<TrafficLight>> trafficLights, SequenceType sequenceType = SequenceType::Auto, LightType lightType = LightType::Red_Yellow_Green, CrossingType crossingType = CrossingType::Standard);
    
    /// @brief Constructs a sequenced system from groups. Each group will follow on from the previous in the order they are given
    /// in the list.
    /// @param trafficLightGroups The groups to sequence.
    /// @param sequenceType The type of sequence. Auto allows the system to advance sequences on its own, and manual allows
    /// for advancing of sequences through requestNextGroup().
    /// @param lightType The type of lighting sequence to use between red and green stages.
    /// @param crossingType The type of crossing present.
    SequencedInterruptableSystem(std::vector<std::shared_ptr<TrafficLightGroup>> trafficLightGroups, SequenceType sequenceType = SequenceType::Auto, LightType lightType = LightType::Red_Yellow_Green, CrossingType crossingType = CrossingType::Standard);

    void requestCrossing();
    void requestNextGroup();
    void setLightType(LightType lightType);
    void setCrossingType(CrossingType crossingType);
    void setSequenceType(SequenceType sequenceType);
    void setTiming(SequencedInterruptableSystemTimings timing, std::chrono::milliseconds time);
    void setTiming(SequencedInterruptableSystemTimings timing, std::chrono::milliseconds time, unsigned int groupId);
    void reset();
    void run() override;

private:
    bool _nextGroupRequested = false;
    bool _crossingRequested = false;

    int _currentGroup = 0;

    LightType _lightType = LightType::Red_Yellow_Green;
    CrossingType _crossingType = CrossingType::Standard;
    SequenceType _sequenceType = SequenceType::Auto;

    std::shared_ptr<TrafficLightGroup> _allLightsGroup;
    std::vector<std::shared_ptr<TrafficLightGroup>> _groups;

    SequencedInterruptableSystem();
    SequencedInterruptableSystem(LightType lightType, CrossingType crossingType, SequenceType sequenceType);

    void setGroups(std::vector<std::shared_ptr<TrafficLightGroup>> groups);
    void setUp();
    void setUpAllLightsGroup();
    void awaitNextGroupRequested();
    void doRedToGreen();
    void doGreenToRed();
    void doCrossingIfRequested();

    bool advanceToNextGroup();

    std::chrono::milliseconds getTimingForCurrentGroup(SequencedInterruptableSystemTimings timing) const;
    std::chrono::milliseconds getStandardTiming(SequencedInterruptableSystemTimings timing) const override;

    std::shared_ptr<TrafficLightGroup> getCurrentGroup() const;
};