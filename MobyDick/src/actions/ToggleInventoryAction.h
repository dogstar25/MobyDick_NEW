#pragma once

#include "Action.h"

class ToggleInventoryAction : public Action
{

public:
	ToggleInventoryAction(Json::Value properties)
		:Action(properties) {}

	void perform(GameObject* doorObject) override;


};

