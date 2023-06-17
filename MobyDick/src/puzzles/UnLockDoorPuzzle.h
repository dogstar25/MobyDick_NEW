#pragma once
#include "Puzzle.h"
class UnLockDoorPuzzle : public Puzzle
{
public:
	UnLockDoorPuzzle() = default;
	virtual bool hasBeenSolved() override;
	virtual bool isPuzzlePieceApplicable(GameObject* puzzlePieceObject) override;
	virtual void applyPuzzlePiece(std::string puzzlePieceId) override;
private:

};

