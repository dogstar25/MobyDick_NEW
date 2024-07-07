#pragma once
#include "Action.h"
class DefaultMoveAction : public Action
{


public:

	DefaultMoveAction(Json::Value properties)
		:Action(properties) {}

	void perform(GameObject* gameObject, int direction, int strafe) override;
};

