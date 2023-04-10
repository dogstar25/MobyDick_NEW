#pragma once
#include "MobyDick.h"

class ActorStairsMoveAction : public Action
{

public:
	ActorStairsMoveAction() = default;

	void perform(GameObject* gameObject, int direction) override;

};