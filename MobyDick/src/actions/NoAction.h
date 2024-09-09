#pragma once
#include "Action.h"
#include <string>

class NoAction :   public Action
{
public:

	NoAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}
	void perform() override {};

};

