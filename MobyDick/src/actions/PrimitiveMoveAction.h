#pragma once
#include "Action.h"
#include <SDL2/SDL.h>

class PrimitiveMoveAction : public Action
{

public:

	PrimitiveMoveAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform(SDL_FRect* gameObjectRect, glm::vec2, float force) override;

};
