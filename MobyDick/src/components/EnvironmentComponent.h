#pragma once
#include "Component.h"
#include <optional>
#include <set>
#include <map>
#include <variant>
#include <unordered_map>
#include <queue>
#include "../BaseConstants.h"

#include "../GameObject.h"

namespace EnvironmentEventType {

	inline constexpr int WAIT_EVENT = 0;
}

enum class EventStatus {

	IDLE,
	IN_PROGRESS,
	COMPLETE

};

enum class EnvironmentEventSubType {

	DISPLAY,
	SOUND_ONETIME,
	SOUND_LOOP

};

struct EnvironmentEvent {

	int environmentEventType;
	Timer durationTimer;
	EventStatus status{ EventStatus::IDLE };

};



class EnvironmentComponent : public Component
{

public:
	EnvironmentComponent(Json::Value definitionJSON);
	~EnvironmentComponent() = default;

	virtual void update() override;
	virtual void postInit() override;
	virtual void setParent(GameObject* gameObject) override;

	virtual const int CYCLE_COUNT() const {
		return 1;
	}

protected:

	//This is a special sequence that will ghe executed immediately, in addition to the normal cycle sequence
	std::queue<EnvironmentEvent> m_oneOffSequence{};

	//Each enviornment cycle item represent an enviornment time lapse of enviuronment events
	//This is used to represent the cycle of maybe a day. Each cycle item will execute in order and start over when reaching the end
	std::vector<std::vector<EnvironmentEvent>> m_enviornmentCycle{};

	int m_currentSequence{ 0 };

private:

	


};

