#pragma once

#include "Action.h"

class TakeItemAction : public Action
{
	public:

	TakeItemAction(Json::Value properties)
		:Action(properties) {}

	void perform(GameObject* doorObject) override;


};

