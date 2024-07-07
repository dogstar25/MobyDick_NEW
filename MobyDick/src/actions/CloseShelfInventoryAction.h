#pragma once

#include "Action.h"

class CloseShelfInventoryAction : public Action
{

public:
	CloseShelfInventoryAction(Json::Value properties)
		:Action(properties) {}

	void perform(GameObject* doorObject) override;


};

