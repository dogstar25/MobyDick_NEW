#pragma once
#include "Action.h"
class DefaultOnHoverOutAction : public Action
{

public:

	DefaultOnHoverOutAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform(GameObject* gameObject) override;

private:


};

