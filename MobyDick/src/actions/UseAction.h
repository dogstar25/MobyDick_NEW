#pragma once
#include "Action.h"
class UseAction : public Action
{

public:

	UseAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	virtual void perform(GameObject* gameObject) {};

private:

};

