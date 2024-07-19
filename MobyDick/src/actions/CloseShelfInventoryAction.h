#pragma once

#include "Action.h"

class CloseShelfInventoryAction : public Action
{

public:
	CloseShelfInventoryAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform(GameObject* doorObject) override;


};

