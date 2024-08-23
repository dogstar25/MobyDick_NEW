#include "UnlockContainer.h"

UnlockContainer::UnlockContainer(std::string name, std::string clue, int pieceCount) :
	Puzzle(name, clue, pieceCount) {

}


bool UnlockContainer::isPuzzlePieceApplicable(GameObject* puzzlePieceObject)
{

	bool isApplicable = false;

	//Piece 1
	if (puzzlePieceObject->type() == "KEY1") {

		isApplicable = true;
	}

	return isApplicable;

}

void UnlockContainer::applyPuzzlePiece(GameObject* puzzleObject, GameObject* puzzlePieceObject)
{

	if (hasBeenSolved() == false) {

		//Piece 1
		if (puzzlePieceObject->type() == "KEY1") {

			m_pieces[0] = true;

		}
	}

	puzzlePieceObject->setRemoveFromWorld(true);

	if (hasBeenSolved() == true) {

		puzzleObject->removeState(GameObjectState::LOCKED);

	}

}
