#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "controller.h"
#include "trafficlight.h"
#include "trafficlight_group.h"
#include "sequence.h"

void initPin(uint pinId, int direction = GPIO_OUT) {
    gpio_init(pinId);
    gpio_set_dir(pinId, GPIO_OUT);
}

void lightsThread()
{
    auto controller = std::make_shared<Controller>();
    auto trafficLight1 = std::make_shared<TrafficLight>(0u, 1u, 2u, 3u, 4u, TrafficLight::LedType::CommonAnode);
    auto trafficLight2 = std::make_shared<TrafficLight>(5u, 6u, 7u, 8u, 9u, TrafficLight::LedType::CommonAnode);
    auto group1 = std::make_shared<TrafficLightGroup>();
    auto group2 = std::make_shared<TrafficLightGroup>();
    auto basicSequence = std::make_shared<Sequence>();

    group1->addTrafficLight(trafficLight1);
    group2->addTrafficLight(trafficLight2);

    basicSequence->add((TrafficLight::Light)(TrafficLight::Light::Red | TrafficLight::Light::RedCrossing), std::chrono::seconds(2));
    basicSequence->add((TrafficLight::Light)(TrafficLight::Light::Red | TrafficLight::Light::Yellow | TrafficLight::Light::RedCrossing), std::chrono::seconds(3));
    basicSequence->add((TrafficLight::Light)(TrafficLight::Light::Green | TrafficLight::Light::RedCrossing), std::chrono::seconds(5));
    basicSequence->add((TrafficLight::Light)(TrafficLight::Light::Yellow | TrafficLight::Light::RedCrossing), std::chrono::seconds(2));
    basicSequence->add((TrafficLight::Light)(TrafficLight::Light::Red | TrafficLight::Light::RedCrossing), std::chrono::seconds(2));

    controller->addTrafficLightGroup(group1, 0);
    controller->addTrafficLightGroup(group2, 1);

    controller->addSequence(basicSequence, 0);
    controller->addSequence(basicSequence, 1);

    while (true) {
        controller->run();
    }
}

void inputsThread()
{
    initPin(13, GPIO_IN);
    
    while (true) {   
        if (gpio_get(13))
        {
        }
        sleep_ms(5);
    }
}

int main() 
{
    lightsThread();
}