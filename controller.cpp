#include "sequence.h"
#include "trafficlight_group.h"
#include "controller.h"

Controller::Controller()
{
}

void Controller::addTrafficLightGroup(std::shared_ptr<TrafficLightGroup> group, unsigned int id)
{
    _groups[id] = group;
}

void Controller::addSequence(std::shared_ptr<Sequence> sequence, unsigned int targetGroupId)
{
    _sequences.push_back({sequence, targetGroupId});
}

void Controller::clearSequences()
{
    _sequences.clear();
}

void Controller::run()
{
    if (count() > 0) {
        do
        {
            auto sequence = getCurrentSequence();
            auto group = getCurrentGroup();

            if (sequence && group) {
                sequence->restart();

                if (sequence->count() > 0) {                
                    do {
                        auto lightsToChange = sequence->getCurrentLight();
                        auto delay = sequence->getCurrentDelay();

                        group->turnAllLightsOff();
                        group->turnLightsOn(lightsToChange);

                        sleep_ms(delay.count());
                    }
                    while (sequence->next());
                }
            }
        }
        while (next());
    }
}

void Controller::reset()
{
    _index = 0;
}

bool Controller::next()
{    
    ++_index;

    if (_index >= count()) {
        reset();
        return false;
    }

    return true;
}

unsigned int Controller::getCurrentGroupId() const
{
    return std::get<1>(getCurrent());
}

size_t Controller::count() const
{
    return _sequences.size();
}

std::tuple<std::shared_ptr<Sequence>, unsigned int> Controller::getCurrent() const
{
    return _sequences[_index];
}

std::shared_ptr<Sequence> Controller::getCurrentSequence() const
{
    return std::get<0>(getCurrent());
}

std::shared_ptr<TrafficLightGroup> Controller::getGroup(size_t id) const
{
    auto group = _groups.find(id);
    if (group != _groups.end()) {
        return group->second;
    }

    return nullptr;
}

std::shared_ptr<TrafficLightGroup> Controller::getCurrentGroup() const
{
    return getGroup(getCurrentGroupId());
}