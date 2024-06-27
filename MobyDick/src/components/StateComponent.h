#pragma once
#include "Component.h"
#include <optional>
#include <set>
#include <map>
#include <unordered_map>
#include "../BaseConstants.h"

#include "../GameObject.h"


struct StateTransition {

	GameObjectState fromState{};
	GameObjectState toState{};
	float transitionDuration{};
	std::optional<Timer> transitionTimer{};
	std::optional<std::string> animationId{};

};

struct RenderState {

	GameObjectState state{};
	std::string animationId{};

};

class StateComponent : public Component
{

public:
	StateComponent(Json::Value definitionJSON, GameObject* parent);
	~StateComponent() = default;

	virtual void update() override;
	virtual void postInit() override;
	//virtual void setParent(GameObject* gameObject) override;

	virtual void addState(GameObjectState state);
	virtual void removeState(GameObjectState state);
	virtual bool testState(GameObjectState state);

	void clear() { m_states.reset(); }

	std::optional<StateTransition> getCurrentTransition();
	std::optional<std::string> getCurrentAnimatedState();
	const std::bitset< static_cast<int>(GameObjectState::GameObjectState_Count)> getStateBitSet() { return m_states; }

protected:

	int m_beginState{};
	std::bitset< static_cast<int>(GameObjectState::GameObjectState_Count)> m_states;
	std::vector< StateTransition> m_transitions;
	std::unordered_map<GameObjectState, RenderState> m_animationStates;

	virtual bool _hasTransitionDuration(GameObjectState state);
	void _setAndReconcileState(GameObjectState State);

private:

	
	

	


};

