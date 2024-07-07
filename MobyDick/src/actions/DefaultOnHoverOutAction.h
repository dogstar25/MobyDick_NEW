#pragma once
#include "Action.h"
class DefaultOnHoverOutAction : public Action
{

public:

	DefaultOnHoverOutAction(Json::Value properties)
		:Action(properties) {}

	void perform(GameObject* gameObject) override;

private:


};

