#pragma once
#include "Action.h"

class DefaultOnHoverAction : public Action
{

public:

	DefaultOnHoverAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform() override;

private:



};

