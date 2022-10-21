#include "sequence.h"

Sequence::Sequence()
{
}

void Sequence::add(TrafficLight::Light light, std::chrono::milliseconds delay)
{
    _sequences.push_back({light, delay});
}

void Sequence::clear()
{
    _sequences.clear();
}

void Sequence::restart()
{
    _index = 0;
}

bool Sequence::next()
{
    ++_index;

    if (_index >= count()) {
        restart();
        return false;
    }

    return true;
}

size_t Sequence::count() const
{
    return _sequences.size();
}

TrafficLight::Light Sequence::getLightForIndex(size_t index) const
{
    if (index < count()) {
        return std::get<0>(_sequences[index]);
    }

    return TrafficLight::None;
}

TrafficLight::Light Sequence::getCurrentLight() const
{
    return getLightForIndex(_index);
}

std::chrono::milliseconds Sequence::getDelayForIndex(size_t index) const
{
    if (index < count()) {
        return std::get<1>(_sequences[index]);
    }

    return std::chrono::milliseconds(0);
}

std::chrono::milliseconds Sequence::getCurrentDelay() const
{
    return getDelayForIndex(_index);
}

std::tuple<TrafficLight::Light, std::chrono::milliseconds> Sequence::getCurrent() const
{
    return _sequences[_index];
}
