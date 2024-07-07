#pragma once

#include "Action.h"

class ButtonCloseInventoryAction : public Action
{

public:
	ButtonCloseInventoryAction(Json::Value properties)
		:Action(properties) {}

	void perform(GameObject* doorObject) override;


};

