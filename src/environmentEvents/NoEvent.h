#pragma once
#include "NoEvent.h"
#include "EnvironmentEvent.h"
#include <string>
#include <vector>

class NoEvent :   public EnvironmentEvent
{
public:
	NoEvent() = default;

	std::vector<std::shared_ptr<EnvironmentEvent>> perform(GameObject* gameObject) override { return std::vector<std::shared_ptr<EnvironmentEvent>>(); }
	void stop(GameObject* gameObject) override {};

};

