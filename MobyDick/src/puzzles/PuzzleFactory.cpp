#include "PuzzleFactory.h"
#include "UnLockDoorPuzzle.h"



std::shared_ptr<Puzzle> PuzzleFactory::create(std::string puzzleType)
{
	std::shared_ptr<Puzzle> puzzle;

	if (puzzleType == "UnlockDoorPuzzle") {

		puzzle = std::make_shared<UnLockDoorPuzzle>();
	}
	else {

		assert(false && "Puzzle Type was not found");
	}

	//Currently have no base level triggers
	assert(false && "Puzzle Type was not found");

	return puzzle;

}
