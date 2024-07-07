#pragma once
#include "Action.h"
class DefaultRotateAction : public Action
{

public:

	DefaultRotateAction(Json::Value properties)
		:Action(properties) {}

	void perform(GameObject* gameObject, float angularVelocity) override;

};

