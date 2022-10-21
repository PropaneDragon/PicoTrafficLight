
#pragma once

#include <memory>
#include <vector>
#include <map>

class TrafficLightGroup;
class Sequence;

class Controller
{
public:
    Controller();

    void addTrafficLightGroup(std::shared_ptr<TrafficLightGroup> group, unsigned int id);
    void addSequence(std::shared_ptr<Sequence> sequence, unsigned int targetGroupId);
    void clearSequences();
    void run();

private:
    size_t _index = 0;

    std::map<unsigned int, std::shared_ptr<TrafficLightGroup>> _groups;
    std::vector<std::tuple<std::shared_ptr<Sequence>, unsigned int>> _sequences;

    void reset();

    bool next();

    unsigned int getCurrentGroupId() const;

    size_t count() const;

    std::tuple<std::shared_ptr<Sequence>, unsigned int> getCurrent() const;
    std::shared_ptr<Sequence> getCurrentSequence() const;
    std::shared_ptr<TrafficLightGroup> getGroup(size_t id) const;
    std::shared_ptr<TrafficLightGroup> getCurrentGroup() const;
};