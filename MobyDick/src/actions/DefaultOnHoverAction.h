#pragma once
#include "Action.h"

class DefaultOnHoverAction : public Action
{

public:

	DefaultOnHoverAction(Json::Value properties)
		:Action(properties) {}

	void perform(GameObject* gameObject) override;

private:



};

