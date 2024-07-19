#pragma once

#include "Action.h"

class ToggleInventoryAction : public Action
{

public:
	ToggleInventoryAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform(GameObject* doorObject) override;


};

