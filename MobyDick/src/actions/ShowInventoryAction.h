#pragma once
#include "Action.h"
class ShowInventoryAction : public Action
{

public:
	ShowInventoryAction() = default;

	void perform(GameObject* gameObject, float angularVelocity) override;

};

