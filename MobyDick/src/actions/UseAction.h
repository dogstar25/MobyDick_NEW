#pragma once
#include "Action.h"
class UseAction : public Action
{

public:

	UseAction(Json::Value properties)
		:Action(properties) {}

	virtual void perform(GameObject* gameObject) {};

private:

};

