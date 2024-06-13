#pragma once
#include "Component.h"
#include "../puzzles/Puzzle.h"
#include <optional>

class PuzzleComponent :  public Component
{

public:
	PuzzleComponent(Json::Value definitionJSON);
	~PuzzleComponent() = default;

	std::shared_ptr<Puzzle> puzzle{};

	bool hasBeenSolved();
	bool isApplicable(GameObject* puzzlePieceObject);

	void update() override;
	void render();

private:


};



