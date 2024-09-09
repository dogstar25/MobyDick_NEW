#pragma once
#include "Action.h"
class ActorWarpAction : public Action
{


public:
	ActorWarpAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform(b2Vec2 destination) override;
};

