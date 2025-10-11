#include "UnlockContainer.h"

UnlockContainer::UnlockContainer(std::string name, std::string clue, int pieceCount, GameObject* parent) :
	Puzzle(name, clue, pieceCount, parent) {

}


bool UnlockContainer::isPuzzlePieceApplicable(GameObject* puzzlePieceObject)
{

	bool isApplicable = false;

	//Since this is a generic unlock puzzle, we have to figure out if the puzzlePieceObject is applicable
	//based on it's name and the name of the puzzle
	std::string keyName = "key_" + parent()->name();


	//Piece 1
	if (puzzlePieceObject->name() == keyName) {

		isApplicable = true;
	}

	return isApplicable;

}

void UnlockContainer::applyPuzzlePiece(GameObject* puzzleObject, GameObject* puzzlePieceObject)
{

	if (hasBeenSolved() == false) {

		//Since this is a generic unlock puzzle, we have to figure out if the puzzlePieceObject is applicable
		//based on it's name and the name of the puzzle
		std::string keyName = "key_" + parent()->name();
		if (puzzlePieceObject->name() == keyName) {

			m_pieces[0] = true;

		}
	}

	puzzlePieceObject->setRemoveFromWorld(true);

	if (hasBeenSolved() == true) {

		puzzleObject->removeState(GameObjectState::LOCKED);

	}

}
