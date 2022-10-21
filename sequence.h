#pragma once

#include <vector>
#include <list>
#include <tuple>
#include <chrono>

#include "trafficlight.h"

class Sequence
{
public:
    Sequence();

    void add(TrafficLight::Light light, std::chrono::milliseconds delay);
    void clear();
    void restart();

    bool next();

    size_t count() const;

    TrafficLight::Light getLightForIndex(size_t index) const;
    TrafficLight::Light getCurrentLight() const;
    std::chrono::milliseconds getDelayForIndex(size_t index) const;
    std::chrono::milliseconds getCurrentDelay() const;
    
    std::tuple<TrafficLight::Light, std::chrono::milliseconds> getCurrent() const;

private:
    size_t _index = 0;

    std::vector<std::tuple<TrafficLight::Light, std::chrono::milliseconds>> _sequences;
};