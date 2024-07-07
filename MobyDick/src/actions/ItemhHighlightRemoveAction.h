#pragma once
#include "Action.h"


class ItemhHighlightRemoveAction : public Action
{
public:
	ItemhHighlightRemoveAction(Json::Value properties)
		:Action(properties) {}

	void perform(GameObject* gameObject) override;


};

