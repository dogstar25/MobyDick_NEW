#pragma once
#include "Component.h"
#include <optional>
#include <set>
#include <map>

#include "../GameObject.h"
#include "../stateRules/baseStateRules.h"


struct State {

	GameObjectState state{};
	float transitionDuration{};
	Timer transitionTimer{};

	// Define the less-than operator for ordering in the set
	//Allows for using .contains and ,erase
	bool operator<(const State& other) const {
		return state < other.state;
	}

};

//not working
//i need to overide this in game
inline State idleState = { GameObjectState::IDLE, 0. };



class StateComponent : public Component
{
public:
	StateComponent(Json::Value definitionJSON);
	~StateComponent() = default;

	virtual void update() override;
	virtual void postInit() override;
	virtual void setParent(GameObject* gameObject) override;

protected:

	virtual void transitionTo(GameObjectState gameObjectState);
	virtual void setState(GameObjectState state);
	virtual void initializeMutuallyExclusiveStates();

	std::set<State> m_states;
	//std::unordered_map<GameObjectState, StateRules> rules;

private:
	

	


};

