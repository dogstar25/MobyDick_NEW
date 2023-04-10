#pragma once
#include "MobyDick.h"
#include "../GameConstants.h"

class ActorMoveAction : public Action
{

public:
	ActorMoveAction() = default;

	void perform(GameObject* gameObject, int direction, int strafe) override;

};