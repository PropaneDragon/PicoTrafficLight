#include <chrono>
#include <map>
#include <thread>
#include <vector>

#include "pico/stdlib.h"
#include "pico/multicore.h"

enum class Lights
{
    LED,
    Red,
    Yellow,
    Green,
    CrossingRed,
    CrossingGreen
};

std::map<Lights, uint> _lightsMap
{
    {Lights::LED, PICO_DEFAULT_LED_PIN},
    {Lights::Red, 0u},
    {Lights::Yellow, 1u},
    {Lights::Green, 2u},
    {Lights::CrossingRed, 3u},
    {Lights::CrossingGreen, 4u}
};

std::vector<Lights> _trafficLights
{
    Lights::Red, Lights::Yellow, Lights::Green, Lights::CrossingRed, Lights::CrossingGreen
};

bool _awaitingCrossing = false;

void setLight(uint pinId, bool on = true)
{
    gpio_put(pinId, !on);
}

void setLight(Lights light, bool on = true)
{
    setLight(_lightsMap.at(light), on);
}

void lightsOff()
{
    for (auto light : _trafficLights)
    {
        setLight(light, false);
    }
}

void flashCrossingToGreen(int flashes = 8)
{
    for (int flash = flashes; flash > 0; flash--)
    {        
        lightsOff();
        setLight(Lights::CrossingGreen);
        setLight(Lights::Yellow);
        sleep_ms(500);
        lightsOff();
        sleep_ms(500);
    }

    lightsOff();
    setLight(Lights::CrossingRed);
    setLight(Lights::Green);
}

void crossingGreen()
{
    lightsOff();    
    setLight(Lights::CrossingGreen);
    setLight(Lights::Red);
}

void crossingRed()
{
    lightsOff();    
    setLight(Lights::CrossingRed);
    setLight(Lights::Red);
}

void transitionToRed(std::chrono::milliseconds yellowMs = std::chrono::seconds(3))
{
    lightsOff();
    setLight(Lights::CrossingRed);
    setLight(Lights::Yellow);
    sleep_ms(yellowMs.count()); 
    lightsOff();
    setLight(Lights::CrossingRed);
    setLight(Lights::Red);
}

void transitionToGreen(std::chrono::milliseconds yellowMs = std::chrono::seconds(3))
{
    lightsOff();
    setLight(Lights::CrossingRed);
    setLight(Lights::Red);
    setLight(Lights::Yellow);
    sleep_ms(yellowMs.count()); 
    lightsOff();
    setLight(Lights::CrossingRed);
    setLight(Lights::Green);
}

void initPin(uint pinId, int direction = GPIO_OUT) {
    gpio_init(pinId);
    gpio_set_dir(pinId, GPIO_OUT);
}

void initLights() 
{
    for (auto lightMap : _lightsMap)
    {
        initPin(lightMap.second);
    }
}

void beginCrossingStage()
{
    transitionToRed();
    sleep_ms(2000);
    crossingGreen();
    sleep_ms(4000);
    flashCrossingToGreen();
}

void lightsThread()
{
    while (true) {
        sleep_ms(100);
        if (_awaitingCrossing) {
            beginCrossingStage();
            _awaitingCrossing = false;
            sleep_ms(5000);
        }
    }
}

void inputsThread()
{
    initPin(13, GPIO_IN);
    
    while (true) {   
        if (!_awaitingCrossing && gpio_get(13))
        {
            _awaitingCrossing = true;
        }         

        gpio_put(_lightsMap[Lights::LED], gpio_get(13));
        sleep_ms(5);
    }
}

int main() 
{
    initLights();
    lightsOff();
    
    multicore_launch_core1(&inputsThread);

    gpio_put(_lightsMap[Lights::Red], 1);
    gpio_put(_lightsMap[Lights::CrossingRed], 1);
    sleep_ms(5000);
    transitionToGreen();
    lightsThread();
}