#pragma once

#include <memory>
#include <list>
#include <vector>
#include <map>

class TrafficLight;
class TrafficLightGroup;
class Sequence;

class StandardSequenceInterruptableSystem
{
public:
    enum class LightType { Red_Yellow_Green, Red_Green };
    enum class CrossingType { None, Standard };
    enum class SequenceType { Auto, Manual };
    enum class Timings { DelayUntilGreenLight, MinimumTimeUntilRedLight, DelayUntilGreenCrossing, CrossingTime };

    StandardSequenceInterruptableSystem(std::vector<std::shared_ptr<TrafficLight>> trafficLights, SequenceType sequenceType = SequenceType::Auto, LightType lightType = LightType::Red_Yellow_Green, CrossingType crossingType = CrossingType::Standard);
    StandardSequenceInterruptableSystem(std::vector<std::shared_ptr<TrafficLightGroup>> trafficLightGroups, SequenceType sequenceType = SequenceType::Auto, LightType lightType = LightType::Red_Yellow_Green, CrossingType crossingType = CrossingType::Standard);

    void requestCrossing();
    void requestNextGroup();
    void setLightType(LightType lightType);
    void setCrossingType(CrossingType crossingType);
    void setSequenceType(SequenceType sequenceType);
    void setTiming(Timings timing, std::chrono::milliseconds time);
    void setTiming(Timings timing, std::chrono::milliseconds time, unsigned int groupId);
    void reset();
    void run();

private:
    bool _nextGroupRequested = false;
    bool _crossingRequested = false;

    int _currentGroup = 0;

    LightType _lightType = LightType::Red_Yellow_Green;
    CrossingType _crossingType = CrossingType::Standard;
    SequenceType _sequenceType = SequenceType::Auto;

    std::shared_ptr<TrafficLightGroup> _allLightsGroup;
    std::vector<std::shared_ptr<TrafficLightGroup>> _groups;
    std::map<unsigned int, std::map<Timings, int>> _delays;

    StandardSequenceInterruptableSystem();
    StandardSequenceInterruptableSystem(LightType lightType, CrossingType crossingType, SequenceType sequenceType);

    void addGroups(std::vector<std::shared_ptr<TrafficLightGroup>> groups);
    void setUp();
    void setUpAllLightsGroup();
    void awaitNextGroupRequested();
    void doRedToGreen();
    void doGreenToRed();
    void doCrossingIfRequested();

    bool advanceToNextGroup();

    std::chrono::milliseconds getTimingForGroup(Timings timing, unsigned int groupId) const;
    std::chrono::milliseconds getTimingForCurrentGroup(Timings timing) const;
    std::chrono::milliseconds getStandardTiming(Timings timing) const;

    std::shared_ptr<TrafficLightGroup> getCurrentGroup() const;
};