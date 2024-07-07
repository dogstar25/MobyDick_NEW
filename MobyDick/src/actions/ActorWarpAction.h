#pragma once
#include "Action.h"
class ActorWarpAction : public Action
{


public:
	ActorWarpAction(Json::Value properties)
		:Action(properties) {}

	void perform(GameObject* interactingObject, GameObject* interactionObject, b2Vec2 destination) override;
};

