#pragma once
#include "Action.h"

#include <box2d/box2d.h>

class DroneMoveAction : public Action
{

public:

	DroneMoveAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform(b2Vec2 trajectory) override;


};


