#include "EnvironmentEventFactory.h"
#include "NoEvent.h"
#include "SoundLoopEvent.h"
#include "SoundEvent.h"

#include <variant>

#include <iostream>


std::shared_ptr<EnvironmentEvent> EnvironmentEventFactory::create(std::string eventType, std::variant<std::string, EventLevel, float> param, Timer durationTimer)
{
    std::shared_ptr<EnvironmentEvent> event;

    if (eventType == "noEvent") {

        event = std::make_shared<NoEvent>();
    }
    else if (eventType == "soundLoop") {

        event = std::make_shared<SoundLoopEvent>(param, durationTimer);
    }
    else if (eventType == "soundEvent") {

        event = std::make_shared<SoundEvent>(param, durationTimer);
    }

    else {

        std::cout << "No Event was Matched" << std::endl;
        event = std::make_shared<NoEvent>();

    }

    return event;
}
