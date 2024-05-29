#pragma once
#include "Component.h"
#include <optional>
#include <set>
#include <map>
#include <variant>
#include <unordered_map>
#include <deque>
#include "../BaseConstants.h"
#include "../environmentEvents/EnvironmentEvent.h"	
#include <variant>

#include "../GameObject.h"

namespace EnvironmentEventType {

	inline constexpr int CYCLE_TIMER = 0;
	inline constexpr int SOUND_LOOP = 1;
}

namespace EnvironmentEventId {

	inline constexpr int NONE = 0;

}

enum class EventLevel {

	NA,
	MILD,
	MODERATE,
	INTENSE

};

struct EnvironmentEventInstruction {

	std::string id;
	std::variant<std::string, EventLevel, float> param;
	Timer durationTimer;


};


class EnvironmentComponent : public Component
{

public:
	EnvironmentComponent(Json::Value definitionJSON);
	~EnvironmentComponent() = default;

	virtual void update() override;
	virtual void postInit() override;
	virtual void setParent(GameObject* gameObject) override;
	void addEvent(std::shared_ptr<EnvironmentEvent> event);


protected:

	std::deque<std::shared_ptr<EnvironmentEvent>> m_events{};
	int m_currentSequence{};
	int m_sequenceCount{};


	std::vector <std::vector<EnvironmentEventInstruction>> m_currentCycleInstructions{};

	virtual void _applyEventInstructions(int sequence);
	bool isEventDequeEmpty();

	void startCycle();


private:

	
	

	


};

