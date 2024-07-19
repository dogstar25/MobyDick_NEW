#pragma once
#include "Action.h"


class ItemhHighlightRemoveAction : public Action
{
public:
	ItemhHighlightRemoveAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform(GameObject* gameObject) override;


};

