#pragma once
#include "Action.h"
class DefaultRotateAction : public Action
{

public:

	DefaultRotateAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform( float angularVelocity) override;

};

