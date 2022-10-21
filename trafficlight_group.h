#pragma once

#include <vector>
#include <memory>

#include "trafficlight.h"

class TrafficLightGroup
{
public:
    TrafficLightGroup();
    TrafficLightGroup(std::vector<std::shared_ptr<TrafficLight>> trafficLights);

    void addTrafficLight(std::shared_ptr<TrafficLight> trafficLight);
    void turnAllLightsOff();
    void turnLightsOn(TrafficLight::Light lights);
    void turnLightsOff(TrafficLight::Light lights);
    void setLightsState(TrafficLight::Light lights, bool on);

    std::vector<std::shared_ptr<TrafficLight>> getTrafficLights() const;

private:
    std::vector<std::shared_ptr<TrafficLight>> _trafficLights;
};