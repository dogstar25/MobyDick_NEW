#pragma once
#include "Action.h"
class DefaultMoveAction : public Action
{


public:

	DefaultMoveAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform(GameObject* gameObject, int direction, int strafe) override;
};

