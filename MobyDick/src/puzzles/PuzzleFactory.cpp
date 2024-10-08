#include "PuzzleFactory.h"
#include "UnLockDoorPuzzle.h"
#include "UnLockContainer.h"



std::shared_ptr<Puzzle> PuzzleFactory::create(Json::Value puzzleJSON, GameObject* parent)
{
	std::shared_ptr<Puzzle> puzzle;

	std::string name = puzzleJSON["name"].asString();
	std::string clue = puzzleJSON["clue"].asString();
	int puzzlePieces = puzzleJSON["pieces"].asInt();
	std::string puzzleType = puzzleJSON["puzzleType"].asString();


	if (puzzleType == "UnlockDoorPuzzle") {

		puzzle = std::make_shared<UnLockDoorPuzzle>(name, clue, puzzlePieces, parent);
	}
	else if (puzzleType == "UnlockContainer") {

		puzzle = std::make_shared<UnlockContainer>(name, clue, puzzlePieces, parent);
	}

	else {

		assert(false && "Puzzle Type was not found");
	}

	return puzzle;

}
