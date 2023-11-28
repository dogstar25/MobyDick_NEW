#pragma once
#include "Component.h"
#include <optional>
#include <set>
#include <map>

#include "../GameObject.h"


struct StateTransition {

	int fromState{};
	int toState{};
	float transitionDuration{};
	std::optional<Timer> transitionTimer{};

};

class StateComponent : public Component
{

public:
	StateComponent(Json::Value definitionJSON);
	~StateComponent() = default;

	virtual void update() override;
	virtual void postInit() override;
	virtual void setParent(GameObject* gameObject) override;

	virtual void addState(int state);
	virtual void removeState(int state);
	virtual bool testState(int state);

protected:

	std::bitset<64> m_states;
	std::vector< StateTransition> m_transitions;

private:
	

	


};

