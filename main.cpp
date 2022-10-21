#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "controller.h"
#include "trafficlight.h"
#include "trafficlight_group.h"
#include "sequence.h"
#include "commonSequences.h"

bool _interruptRequested = false;

void runSimpleExamples(bool loopAutomatically = true)
{
    auto standardController = std::make_shared<Controller>();
    auto standardAlternateController = std::make_shared<Controller>();
    auto usFlashingController = std::make_shared<Controller>();
    auto flashingCrossingController = std::make_shared<Controller>();

    auto northTrafficLight = std::make_shared<TrafficLight>(0u, 1u, 2u, 3u, 4u, TrafficLight::LedType::CommonAnode);
    auto southTrafficLight = std::make_shared<TrafficLight>(5u, 6u, 7u, 8u, 9u, TrafficLight::LedType::CommonAnode);

    auto northTrafficLightGroup = std::make_shared<TrafficLightGroup>(std::vector<std::shared_ptr<TrafficLight>>{northTrafficLight});
    auto southTrafficLightGroup = std::make_shared<TrafficLightGroup>(std::vector<std::shared_ptr<TrafficLight>>{southTrafficLight});
    auto allTrafficLightGroup = std::make_shared<TrafficLightGroup>(std::vector<std::shared_ptr<TrafficLight>>{northTrafficLight, southTrafficLight});

    auto allRedStartSequence = std::make_shared<StaticSequence>(std::chrono::seconds(2));
    auto customAllRedFlashSequence = std::make_shared<StaticSequence>(std::chrono::milliseconds(0));
    auto customAllYellowFlashSequence = std::make_shared<StaticSequence>(std::chrono::milliseconds(500), (TrafficLight::Light)(TrafficLight::Light::Yellow | TrafficLight::Light::RedCrossing));
    auto customOffFlashSequence = std::make_shared<StaticSequence>(std::chrono::milliseconds(500), TrafficLight::Light::RedCrossing);
    auto redToGreenSequence = std::make_shared<RedOrangeToGreenSequence>(std::chrono::seconds(5));
    auto redToGreenAlternateSequence = std::make_shared<RedToGreenSequence>(std::chrono::seconds(5));
    auto greenToRedSequence = std::make_shared<GreenToRedSequence>(std::chrono::seconds(2));
    auto redCrossingToGreenCrossingSequence = std::make_shared<RedCrossingToGreenCrossingSequence>(std::chrono::seconds(5));
    auto greenCrossingToRedCrossingSequence = std::make_shared<GreenCrossingToRedCrossingSequence>(std::chrono::seconds(2));
    auto greenCrossingFlashingSequence = std::make_shared<FlashingCrossingToGreenSequence>(std::chrono::seconds(5));

    standardController->addTrafficLightGroup(allTrafficLightGroup, 0);
    standardController->addTrafficLightGroup(northTrafficLightGroup, 1);
    standardController->addTrafficLightGroup(southTrafficLightGroup, 2);

    standardAlternateController->addTrafficLightGroup(allTrafficLightGroup, 0);
    standardAlternateController->addTrafficLightGroup(northTrafficLightGroup, 1);
    standardAlternateController->addTrafficLightGroup(southTrafficLightGroup, 2);

    usFlashingController->addTrafficLightGroup(allTrafficLightGroup, 0);
    usFlashingController->addTrafficLightGroup(northTrafficLightGroup, 1);
    usFlashingController->addTrafficLightGroup(southTrafficLightGroup, 2);

    flashingCrossingController->addTrafficLightGroup(allTrafficLightGroup, 0);
    flashingCrossingController->addTrafficLightGroup(northTrafficLightGroup, 1);
    flashingCrossingController->addTrafficLightGroup(southTrafficLightGroup, 2);

    standardController->addSequence(allRedStartSequence, 0);
    standardController->addSequence(redToGreenSequence, 1);
    standardController->addSequence(greenToRedSequence, 1);
    standardController->addSequence(redToGreenSequence, 2);
    standardController->addSequence(greenToRedSequence, 2);
    standardController->addSequence(redCrossingToGreenCrossingSequence, 0);
    standardController->addSequence(greenCrossingToRedCrossingSequence, 0);

    standardAlternateController->addSequence(allRedStartSequence, 0);
    standardAlternateController->addSequence(redToGreenAlternateSequence, 1);
    standardAlternateController->addSequence(greenToRedSequence, 1);
    standardAlternateController->addSequence(redToGreenAlternateSequence, 2);
    standardAlternateController->addSequence(greenToRedSequence, 2);
    standardAlternateController->addSequence(redCrossingToGreenCrossingSequence, 0);
    standardAlternateController->addSequence(greenCrossingToRedCrossingSequence, 0);

    usFlashingController->addSequence(customAllRedFlashSequence, 1);
    usFlashingController->addSequence(customAllYellowFlashSequence, 2);
    usFlashingController->addSequence(customOffFlashSequence, 0);

    flashingCrossingController->addSequence(allRedStartSequence, 0);
    flashingCrossingController->addSequence(redToGreenSequence, 0);
    flashingCrossingController->addSequence(greenToRedSequence, 0);
    flashingCrossingController->addSequence(redCrossingToGreenCrossingSequence, 0);
    flashingCrossingController->addSequence(greenCrossingFlashingSequence, 0);
    flashingCrossingController->addSequence(greenToRedSequence, 0);

    while(true) {
        do {
            standardController->run();
        }
        while (!loopAutomatically && !_interruptRequested);

        _interruptRequested = false;

        do {
            standardAlternateController->run();
        }
        while (!loopAutomatically && !_interruptRequested);

        _interruptRequested = false;

        do {
            flashingCrossingController->run();
        }        
        while (!loopAutomatically && !_interruptRequested);

        do {
            for (int count = 20; count > 0; --count) {
                usFlashingController->run();
            }
        }        
        while (!loopAutomatically && !_interruptRequested);
    }
}

void runInterruptableMultiCrossingExample(std::list<std::shared_ptr<TrafficLight>> trafficLights)
{
    std::list<std::shared_ptr<Controller>> individualLightControllers;
    
    auto crossingController = std::make_shared<Controller>();

    auto allTrafficLightGroup = std::make_shared<TrafficLightGroup>();

    auto allRedStartSequence = std::make_shared<StaticSequence>(std::chrono::seconds(2));
    auto redToGreenSequence = std::make_shared<RedOrangeToGreenSequence>(std::chrono::seconds(5));
    auto greenToRedSequence = std::make_shared<GreenToRedSequence>(std::chrono::seconds(2));
    auto redCrossingToGreenCrossingSequence = std::make_shared<RedCrossingToGreenCrossingSequence>(std::chrono::seconds(5));
    auto greenCrossingToRedCrossingSequence = std::make_shared<GreenCrossingToRedCrossingSequence>(std::chrono::seconds(2));

    crossingController->addTrafficLightGroup(allTrafficLightGroup, 0);
    
    crossingController->addSequence(allRedStartSequence, 0);
    crossingController->addSequence(redCrossingToGreenCrossingSequence, 0);
    crossingController->addSequence(greenCrossingToRedCrossingSequence, 0);

    for (auto trafficLight : trafficLights) {
        auto lightController = std::make_shared<Controller>();
        auto lightGroup = std::make_shared<TrafficLightGroup>();

        lightGroup->addTrafficLight(trafficLight);
        allTrafficLightGroup->addTrafficLight(trafficLight);

        lightController->addTrafficLightGroup(allTrafficLightGroup, 0);
        lightController->addTrafficLightGroup(lightGroup, 1);

        lightController->addSequence(allRedStartSequence, 0);
        lightController->addSequence(redToGreenSequence, 1);
        lightController->addSequence(greenToRedSequence, 1);

        individualLightControllers.push_back(lightController);
    }

    while(true)
    {
        for (auto controller : individualLightControllers)
        {
            controller->run();

            if (_interruptRequested) {
                crossingController->run();
                _interruptRequested = false;
            }
        }
    }
}

void runInterruptableSingleCrossingExample(std::list<std::shared_ptr<TrafficLight>> trafficLights)
{
    auto toGreenController = std::make_shared<Controller>();
    auto toRedController = std::make_shared<Controller>();
    auto crossingController = std::make_shared<Controller>();

    std::list<std::shared_ptr<Controller>> allControllers {toGreenController, toRedController, crossingController};

    auto allTrafficLightGroup = std::make_shared<TrafficLightGroup>();

    auto allRedStartSequence = std::make_shared<StaticSequence>(std::chrono::seconds(2));
    auto redToGreenSequence = std::make_shared<RedOrangeToGreenSequence>(std::chrono::seconds(5));
    auto greenToRedSequence = std::make_shared<GreenToRedSequence>(std::chrono::seconds(2));
    auto redCrossingToGreenCrossingSequence = std::make_shared<RedCrossingToGreenCrossingSequence>(std::chrono::seconds(5));
    auto greenCrossingToRedCrossingSequence = std::make_shared<GreenCrossingToRedCrossingSequence>(std::chrono::seconds(2));

    for (auto trafficLight : trafficLights) {
        allTrafficLightGroup->addTrafficLight(trafficLight);
    }

    for (auto controller : allControllers) {
        controller->addTrafficLightGroup(allTrafficLightGroup, 0);
    }

    toGreenController->addSequence(allRedStartSequence, 0);
    toGreenController->addSequence(redToGreenSequence, 0);

    toRedController->addSequence(greenToRedSequence, 0);

    crossingController->addSequence(allRedStartSequence, 0);
    crossingController->addSequence(redCrossingToGreenCrossingSequence, 0);
    crossingController->addSequence(greenCrossingToRedCrossingSequence, 0);

    toGreenController->run();

    while (true) {
        if (_interruptRequested) {
            toRedController->run();
            crossingController->run();
            toGreenController->run();

            _interruptRequested = false;
        }

        sleep_ms(1000);
    }
}

void lightsThread()
{
    auto northTrafficLight = std::make_shared<TrafficLight>(0u, 1u, 2u, 3u, 4u, TrafficLight::LedType::CommonAnode);
    auto southTrafficLight = std::make_shared<TrafficLight>(5u, 6u, 7u, 8u, 9u, TrafficLight::LedType::CommonAnode);

    //runInterruptableMultiCrossingExample({northTrafficLight, southTrafficLight});
    runInterruptableSingleCrossingExample({northTrafficLight, southTrafficLight});
}

void initPin(uint pinId, int direction = GPIO_OUT) {
    gpio_init(pinId);
    gpio_set_dir(pinId, GPIO_OUT);
}

void inputsThread()
{
    initPin(13, GPIO_IN);
    
    while (true) {   
        if (gpio_get(13))
        {
            _interruptRequested = true;
        }
        sleep_ms(5);
    }
}

int main() 
{
    multicore_launch_core1(&inputsThread);
    lightsThread();
}