#pragma once

#include "Action.h"

class ButtonCloseInventoryAction : public Action
{

public:
	ButtonCloseInventoryAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform(GameObject* doorObject) override;


};

