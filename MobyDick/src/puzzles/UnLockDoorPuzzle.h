#pragma once
#include "Puzzle.h"
class UnLockDoorPuzzle : public Puzzle
{
public:
	UnLockDoorPuzzle() = default;
	UnLockDoorPuzzle(std::string name, std::string clue, int pieceCount);

	virtual bool isPuzzlePieceApplicable(GameObject* puzzlePieceObject) override;
	virtual void applyPuzzlePiece(GameObject* puzzleObject, GameObject* puzzlePieceObject) override;

private:

};

