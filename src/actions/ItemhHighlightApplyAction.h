#pragma once
#include "Action.h"


class ItemhHighlightApplyAction : public Action
{

public:
	ItemhHighlightApplyAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform() override;


};

