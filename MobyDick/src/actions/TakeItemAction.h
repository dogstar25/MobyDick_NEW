#pragma once

#include "Action.h"

class TakeItemAction : public Action
{
	public:

	TakeItemAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform() override;


};

