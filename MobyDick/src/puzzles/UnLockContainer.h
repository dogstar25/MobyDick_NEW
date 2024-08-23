#pragma once
#include "Puzzle.h"
class UnlockContainer : public Puzzle
{
public:
	UnlockContainer() = default;
	UnlockContainer(std::string name, std::string clue, int pieceCount);

	virtual bool isPuzzlePieceApplicable(GameObject* puzzlePieceObject) override;
	virtual void applyPuzzlePiece(GameObject* puzzleObject, GameObject* puzzlePieceObject) override;

private:

};

