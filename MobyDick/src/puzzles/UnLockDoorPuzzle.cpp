#include "UnLockDoorPuzzle.h"

bool UnLockDoorPuzzle::hasBeenSolved()
{
	return false;
}

bool UnLockDoorPuzzle::isPuzzlePieceApplicable(GameObject* puzzlePieceObject)
{

	if (m_pieces.find(puzzlePieceObject->name()) != m_pieces.end()) {
		return true;
	}

	return false;

}

void UnLockDoorPuzzle::applyPuzzlePiece(std::string puzzlePieceId)
{
	Puzzle::applyPuzzlePiece(puzzlePieceId);


	auto iter = m_pieces.find(puzzlePieceId);
	iter->second.isSolved = true;

	//execute the keys "USE" action
	//which will play key unlock sound and animate the key

	//Do other stuff



}
