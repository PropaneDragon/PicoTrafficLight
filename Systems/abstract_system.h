#pragma once

#include <chrono>
#include <map>

template<typename TimingEnum>
class AbstractSystem
{
public:
    virtual void run() = 0; 

protected:
    virtual void setTimingInternal(TimingEnum timingEnum, std::chrono::milliseconds time, int groupId = -1)
    {
        _timings[groupId][timingEnum] = time.count();
    };

    virtual std::chrono::milliseconds getTiming(TimingEnum timing, int groupId = -1) const
    {
        auto foundGroup = _timings.find(groupId);
        if (foundGroup != _timings.end()) {
            auto timingsMap = foundGroup->second;
            auto foundTiming = timingsMap.find(timing);
            if (foundTiming != timingsMap.end()) {
                return std::chrono::milliseconds(foundTiming->second);
            }
        }
        
        return getStandardTiming(timing);
    };

    virtual std::chrono::milliseconds getStandardTiming(TimingEnum timing) const = 0;

private:    
    std::map<int, std::map<TimingEnum, int>> _timings;
};