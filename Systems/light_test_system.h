#pragma once

#include <memory>
#include <vector>

#include "abstract_system.h"

class TrafficLight;

enum class LightTestSystemTimings
{
    AnimationDelay
};

class LightTestSystem : public AbstractSystem<LightTestSystemTimings>
{
public:
    LightTestSystem(std::vector<std::shared_ptr<TrafficLight>> trafficLights) { _trafficLights = trafficLights; }

    void setTiming(LightTestSystemTimings timing, std::chrono::milliseconds delay);
    void run() override;

private:
    std::vector<std::shared_ptr<TrafficLight>> _trafficLights;

    std::chrono::milliseconds getStandardTiming(LightTestSystemTimings timing) const override;
};