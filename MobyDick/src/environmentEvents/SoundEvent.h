#pragma once
#include "SoundLoopEvent.h"
#include "EnvironmentEvent.h"
#include <string>
#include <variant>
#include "../components/EnvironmentComponent.h"
#include "../Timer.h"


class SoundEvent : public EnvironmentEvent
{
public:
	//SoundLoopEvent() = default;
	SoundEvent(std::variant<std::string, EventLevel, float> param, Timer durationTimer);

	std::vector<std::shared_ptr<EnvironmentEvent>> perform(GameObject* gameObject) override;
	void stop(GameObject* gameObject) override;

private:
	std::string m_soundId{};
	std::optional<int> m_soundChannel{};
};

