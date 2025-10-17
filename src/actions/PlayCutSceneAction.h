#pragma once
#include "Action.h"
#include <SDL2/SDL.h>

class PlayCutSceneAction : public Action
{

public:
	PlayCutSceneAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform() override;

};