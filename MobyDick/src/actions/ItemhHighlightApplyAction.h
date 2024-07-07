#pragma once
#include "Action.h"


class ItemhHighlightApplyAction : public Action
{

public:
	ItemhHighlightApplyAction(Json::Value properties)
		:Action(properties) {}

	void perform(GameObject* gameObject) override;


};

