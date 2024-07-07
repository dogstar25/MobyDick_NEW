#pragma once
#include "Action.h"
#include <string>

class NoAction :   public Action
{
public:

	NoAction(Json::Value properties)
		:Action(properties) {}
	void perform(GameObject* gameObject) override {};

};

