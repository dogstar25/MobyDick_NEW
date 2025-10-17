#pragma once

#include <memory>
#include <optional>

#include "EnvironmentEvent.h"
#include "../components/EnvironmentComponent.h"

class EnvironmentEventFactory {



public:

	virtual std::shared_ptr<EnvironmentEvent> create(std::string eventType, std::variant<std::string, EventLevel,float> param, Timer durationTimer);


private:




};
