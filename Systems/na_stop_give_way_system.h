#pragma once

#include <vector>
#include <memory>
#include <map>

#include "abstract_system.h"

class TrafficLight;
class TrafficLightGroup;

enum class NAStopGiveWaySystemTimings
{
    FlashInterval, //The time between flashes
    LoopTime //The amount of time to flash for until resetting
};

/// @brief North American style flashing red/yellow lights.
class NAStopGiveWaySystem : public AbstractSystem<NAStopGiveWaySystemTimings>
{
public:
    NAStopGiveWaySystem(std::vector<std::shared_ptr<TrafficLight>> trafficLights, unsigned int priorityLightId = 0);
    NAStopGiveWaySystem(std::shared_ptr<TrafficLightGroup> priorityTrafficLightGroup, std::shared_ptr<TrafficLightGroup> stopLightGroup);

    void run() override;
    void setTiming(NAStopGiveWaySystemTimings timing, std::chrono::milliseconds time);

private:
    std::shared_ptr<TrafficLightGroup> _priorityGroup;
    std::shared_ptr<TrafficLightGroup> _stopGroup;

    void setGroups(std::shared_ptr<TrafficLightGroup> priority, std::shared_ptr<TrafficLightGroup> stop);
    void flash();

    std::chrono::milliseconds getStandardTiming(NAStopGiveWaySystemTimings timing) const override;
};