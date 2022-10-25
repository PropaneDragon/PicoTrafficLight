#pragma once

#include <vector>
#include <memory>
#include <map>

#include "abstract_system.h"

class TrafficLight;
class TrafficLightGroup;

enum class SingleInterruptableCrossingSystemTimings
{
    DelayAfterCrossingRequest, //The time to wait after a request to cross has been requested before changing.
    DelayBetweenRedLightAndGreenCrossing, //The delay between the red light for vehicles and green light for pedestrians.
    CrossingTime, //The amount of time allocated to allow for pedestrians to cross.
    FlashInterval, //The time between flashes when CrossingStyle is set to Flashing.
    OffTimeBetweenGreenAndRedCrossing, //Time where both red and green crossing lights are off before switching back to red when CrossingStyle is set to Standard.
    DelayBetweenRedCrossingAndGreenLight, //Time after red crossing shows before lights change to green.
    DelayBetweenCrossingRequests //The amount of time to wait in between requests to cross before changing again.
};

/// @brief A system designed to handle a crossing across a single one way or two way road.
class SingleInterruptableCrossingSystem : public AbstractSystem<SingleInterruptableCrossingSystemTimings>
{
public:
    enum class CrossingStyle { Standard, Flashing };
    enum class LightType { Red_Yellow_Green, Red_Green };

    SingleInterruptableCrossingSystem(std::vector<std::shared_ptr<TrafficLight>> trafficLights, CrossingStyle crossingStyle = CrossingStyle::Standard, LightType lightType = LightType::Red_Yellow_Green);
    SingleInterruptableCrossingSystem(std::shared_ptr<TrafficLightGroup> group, CrossingStyle crossingStyle = CrossingStyle::Standard, LightType lightType = LightType::Red_Yellow_Green);
    
    void requestCrossing();
    void setCrossingStyle(CrossingStyle crossingStyle);
    void setLightType(LightType lightType);
    void setTiming(SingleInterruptableCrossingSystemTimings timing, std::chrono::milliseconds time);
    void reset();
    void run() override;

private:
    bool _crossingRequested = false;

    CrossingStyle _crossingStyle = CrossingStyle::Standard;
    LightType _lightType = LightType::Red_Yellow_Green;

    std::shared_ptr<TrafficLightGroup> _lightsGroup;
    std::map<SingleInterruptableCrossingSystemTimings, int> _delays;

    void setTrafficLights(std::vector<std::shared_ptr<TrafficLight>> trafficLights);
    void setUp();
    void doGreen();
    void doGreenToRed();
    void doCrossing();
    void awaitCrossingRequest();

    std::chrono::milliseconds getStandardTiming(SingleInterruptableCrossingSystemTimings timing) const override;
};