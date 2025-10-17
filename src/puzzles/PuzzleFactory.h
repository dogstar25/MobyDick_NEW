#pragma once
#include <memory>
#include <string>
#include <json/json.h>
#include <box2d/box2d.h>
#include <SDL2/SDL.h>
#include "Puzzle.h"

class PuzzleFactory
{
public:
	PuzzleFactory() = default;

	virtual std::shared_ptr<Puzzle> create(Json::Value, GameObject* parent);

private:

};

