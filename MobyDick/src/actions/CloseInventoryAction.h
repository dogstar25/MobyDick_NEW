#pragma once

#include "Action.h"

class CloseInventoryAction : public Action
{

public:
	CloseInventoryAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform(GameObject* doorObject) override;


};

