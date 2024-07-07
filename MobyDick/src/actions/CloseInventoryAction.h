#pragma once

#include "Action.h"

class CloseInventoryAction : public Action
{

public:
	CloseInventoryAction(Json::Value properties)
		:Action(properties) {}

	void perform(GameObject* doorObject) override;


};

