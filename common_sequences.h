#pragma once

#include <chrono>

#include "sequence.h"

class StaticSequence : public Sequence
{
public:
    StaticSequence(std::chrono::milliseconds delay, TrafficLight::Light light = (TrafficLight::Light)(TrafficLight::Light::Red | TrafficLight::Light::RedCrossing))
    {
        add(light, delay);
    }
};

class GreenToRedSequence : public Sequence
{
public:
    GreenToRedSequence(std::chrono::milliseconds delay, std::chrono::milliseconds yellowTime = std::chrono::seconds(3))
    {
        add((TrafficLight::Light)(TrafficLight::Light::Yellow | TrafficLight::Light::RedCrossing), yellowTime);
        add((TrafficLight::Light)(TrafficLight::Light::Red | TrafficLight::Light::RedCrossing), delay);
    }
};

class RedToGreenSequence : public Sequence
{
public:
    enum class SequenceType {Red_Green, Red_Yellow_Green};

    RedToGreenSequence(std::chrono::milliseconds delay, SequenceType sequenceType = SequenceType::Red_Yellow_Green, std::chrono::milliseconds yellowTime = std::chrono::seconds(3))
    {
        if (sequenceType == SequenceType::Red_Yellow_Green) {
            add((TrafficLight::Light)(TrafficLight::Light::Red | TrafficLight::Light::Yellow | TrafficLight::Light::RedCrossing), yellowTime);
        }

        add((TrafficLight::Light)(TrafficLight::Light::Green | TrafficLight::Light::RedCrossing), delay);
    }
};

class RedCrossingToGreenCrossingSequence : public Sequence
{
public:
    RedCrossingToGreenCrossingSequence(std::chrono::milliseconds delay)
    {
        add((TrafficLight::Light)(TrafficLight::Light::Red | TrafficLight::Light::GreenCrossing), delay);
    }
};

class GreenCrossingToRedCrossingSequence : public Sequence
{
public:
    GreenCrossingToRedCrossingSequence(std::chrono::milliseconds delay)
    {
        add((TrafficLight::Light)(TrafficLight::Light::Red | TrafficLight::Light::RedCrossing), delay);
    }
};

class FlashingSequence : public Sequence
{
public:
    FlashingSequence(TrafficLight::Light light, TrafficLight::Light staticLights = TrafficLight::Light::RedCrossing, unsigned int numberOfFlashes = 5, std::chrono::milliseconds flashTime = std::chrono::milliseconds(500))
    {
        addFlashes(light, staticLights, numberOfFlashes, flashTime);
    }

protected:
    FlashingSequence()
    {
    }

    void addFlashes(TrafficLight::Light light, TrafficLight::Light staticLights = TrafficLight::Light::RedCrossing, unsigned int numberOfFlashes = 5, std::chrono::milliseconds flashTime = std::chrono::milliseconds(500))
    {
        for (unsigned int count = 0; count < numberOfFlashes; ++count) {
            addFlash(light, staticLights, flashTime);
        }
    }

    void addFlash(TrafficLight::Light light, TrafficLight::Light staticLights = TrafficLight::Light::RedCrossing, std::chrono::milliseconds flashTime = std::chrono::milliseconds(500))
    {
        add(staticLights, flashTime);
        add(light, flashTime);
    }
};

class FlashingCrossingToGreenSequence : public FlashingSequence
{
public:
    FlashingCrossingToGreenSequence(std::chrono::milliseconds postFlashDelay, unsigned int numberOfFlashes = 5, std::chrono::milliseconds flashTime = std::chrono::milliseconds(500)) : FlashingSequence()
    {
        addFlashes((TrafficLight::Light)(TrafficLight::Light::Yellow | TrafficLight::Light::GreenCrossing), TrafficLight::Light::None, numberOfFlashes, flashTime);
        add((TrafficLight::Light)(TrafficLight::Light::Green | TrafficLight::Light::RedCrossing), postFlashDelay);
    }
};

class YellowFlashingSequence : public FlashingSequence
{
public:
    YellowFlashingSequence(unsigned int numberOfFlashes = 5, std::chrono::milliseconds flashTime = std::chrono::milliseconds(500)) : FlashingSequence(TrafficLight::Light::Yellow, TrafficLight::Light::RedCrossing, numberOfFlashes, flashTime)
    {
    }
};

class RedFlashingSequence : public FlashingSequence
{
public:
    RedFlashingSequence(unsigned int numberOfFlashes = 5, std::chrono::milliseconds flashTime = std::chrono::milliseconds(500)) : FlashingSequence(TrafficLight::Light::Red, TrafficLight::Light::RedCrossing, numberOfFlashes, flashTime)
    {
    }
};

class TestSequence : public Sequence
{
public:
    TestSequence(std::chrono::milliseconds animationTime = std::chrono::milliseconds(300))
    {
        add(TrafficLight::Light::All, animationTime);
        add(TrafficLight::Light::Red, animationTime);
        add(TrafficLight::Light::Yellow, animationTime);
        add(TrafficLight::Light::Green, animationTime);
        add(TrafficLight::Light::RedCrossing, animationTime);
        add(TrafficLight::Light::GreenCrossing, animationTime);
        add(TrafficLight::Light::None, animationTime);
        add(TrafficLight::Light::Red, animationTime);
        add((TrafficLight::Light)(TrafficLight::Light::Red | TrafficLight::Light::Yellow), animationTime);
        add((TrafficLight::Light)(TrafficLight::Light::Red | TrafficLight::Light::Yellow | TrafficLight::Light::Green), animationTime);
        add((TrafficLight::Light)(TrafficLight::Light::Red | TrafficLight::Light::Yellow | TrafficLight::Light::Green | TrafficLight::Light::RedCrossing), animationTime);
        add(TrafficLight::Light::All, animationTime);
        add((TrafficLight::Light)(TrafficLight::Light::Red | TrafficLight::Light::Yellow | TrafficLight::Light::Green | TrafficLight::Light::RedCrossing), animationTime);
        add((TrafficLight::Light)(TrafficLight::Light::Red | TrafficLight::Light::Yellow | TrafficLight::Light::Green), animationTime);
        add((TrafficLight::Light)(TrafficLight::Light::Red | TrafficLight::Light::Yellow), animationTime);
        add(TrafficLight::Light::Red, animationTime);
        add(TrafficLight::Light::None, animationTime);
    }
};