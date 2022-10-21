#include <stdexcept>

#include "pico/stdlib.h"

#include "trafficlight.h"

TrafficLight::TrafficLight(uint redPin, uint yellowPin, uint greenPin, LedType ledType)
{
    setUpForStandardLights(redPin, yellowPin, greenPin);
    setLedType(ledType);
}

TrafficLight::TrafficLight(uint redCrossingPin, uint greenCrossingPin, LedType ledType)
{
    setUpForCrossingLights(redCrossingPin, greenCrossingPin);
    setLedType(ledType);
}

TrafficLight::TrafficLight(uint redPin, uint yellowPin, uint greenPin, uint redCrossingPin, uint greenCrossingPin, LedType ledType)
{
    setUpForStandardLights(redPin, yellowPin, greenPin);
    setUpForCrossingLights(redCrossingPin, greenCrossingPin);
    setLedType(ledType);
}

void TrafficLight::setUpForStandardLights(uint redPin, uint yellowPin, uint greenPin)
{
    setPin(Light::Red, redPin);
    setPin(Light::Yellow, yellowPin);
    setPin(Light::Green, greenPin);

    _hasLights = true;
}

void TrafficLight::setUpForCrossingLights(uint redPin, uint greenPin)
{
    setPin(Light::RedCrossing, redPin);
    setPin(Light::GreenCrossing, greenPin);

    _hasCrossingLights = true;
}

void TrafficLight::setLedType(LedType ledType)
{
    _ledType = ledType;
}

void TrafficLight::turnAllLightsOff()
{
    turnLightsOff(Light::All);
}

void TrafficLight::turnLightsOn(Light lights)
{
    setLightsState(lights, true);
}

void TrafficLight::turnLightsOff(Light lights)
{
    setLightsState(lights, false);
}

void TrafficLight::setLightsState(Light lights, bool on)
{    
    for (auto pinMapping : _lightPinMap) {
        if ((lights & pinMapping.first) != 0 && hasValidPin(pinMapping.first)) {
            gpio_put(pinMapping.second, shouldInvertOnOff() ? !on : on);
        }
    }
}

void TrafficLight::setPin(Light light, uint pin)
{
    _lightPinMap[light] = pin;
    
    initPin(pin);
}

bool TrafficLight::hasLights() const
{
    return _hasLights;
}

bool TrafficLight::hasCrossingLights() const
{
    return _hasCrossingLights;
}

bool TrafficLight::hasValidPin(Light light) const
{
    return _lightPinMap.find(light) != _lightPinMap.end();
}

uint TrafficLight::getPin(Light light) const
{
    auto foundPin = _lightPinMap.find(light);
    if (foundPin != _lightPinMap.end()) {
        return foundPin->second;
    }

    return 0;
}

void TrafficLight::initPin(uint pin)
{
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
}

bool TrafficLight::shouldInvertOnOff() const
{
    return _ledType == LedType::CommonAnode;
}