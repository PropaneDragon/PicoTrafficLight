#include "trafficlight_group.h"

TrafficLightGroup::TrafficLightGroup()
{
}

TrafficLightGroup::TrafficLightGroup(std::vector<std::shared_ptr<TrafficLight>> trafficLights)
{
    for (auto trafficLight : trafficLights) {
        addTrafficLight(trafficLight);
    }
}

void TrafficLightGroup::addTrafficLight(std::shared_ptr<TrafficLight> trafficLight)
{
    _trafficLights.push_back(trafficLight);
}

void TrafficLightGroup::turnAllLightsOff()
{
    for (auto trafficLight : _trafficLights) {
        trafficLight->turnAllLightsOff();
    }
}

void TrafficLightGroup::turnLightsOn(TrafficLight::Light lights)
{
    for (auto trafficLight : _trafficLights) {
        trafficLight->turnLightsOn(lights);
    }
}

void TrafficLightGroup::turnLightsOff(TrafficLight::Light lights)
{
    for (auto trafficLight : _trafficLights) {
        trafficLight->turnLightsOff(lights);
    }
}

void TrafficLightGroup::setLightsState(TrafficLight::Light lights, bool on)
{
    for (auto trafficLight : _trafficLights) {
        trafficLight->setLightsState(lights, on);
    }
}

std::vector<std::shared_ptr<TrafficLight>> TrafficLightGroup::getTrafficLights() const
{
    return _trafficLights;
}