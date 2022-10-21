#pragma once

#include <map>

#include "pico/stdlib.h"

class TrafficLight
{
public:
    enum class LedType
    {
        CommonAnode,
        CommonCathode
    };

    enum Light
    {
        None = 0,

        Red = 1 << 0,
        Yellow = 1 << 1,
        Green = 1 << 2,
        RedCrossing = 1 << 3,
        GreenCrossing = 1 << 4,

        Main = Red | Yellow | Green,
        Crossing = RedCrossing | GreenCrossing,

        All = Main | Crossing
    };

    TrafficLight(uint redPin, uint yellowPin, uint greenPin, LedType ledType = LedType::CommonCathode);
    TrafficLight(uint redPin, uint yellowPin, uint greenPin, uint redCrossingPin, uint greenCrossingPin, LedType ledType = LedType::CommonCathode);
    TrafficLight(uint redCrossingPin, uint greenCrossingPin, LedType ledType = LedType::CommonCathode);

    void setUpForStandardLights(uint redPin, uint yellowPin, uint greenPin);
    void setUpForCrossingLights(uint redPin, uint greenPin);
    void setLedType(LedType ledType);
    void turnAllLightsOff();
    void turnLightsOn(Light lights);
    void turnLightsOff(Light lights);
    void setLightsState(Light lights, bool on);
    void setPin(Light light, uint pin);

    bool hasLights() const;
    bool hasCrossingLights() const;
    bool hasValidPin(Light light) const;

    uint getPin(Light light) const;

private:
    bool _hasLights = false;
    bool _hasCrossingLights = false;

    LedType _ledType = LedType::CommonCathode;
    
    std::map<Light, uint> _lightPinMap;

    void initPin(uint pin);

    bool shouldInvertOnOff() const;
};