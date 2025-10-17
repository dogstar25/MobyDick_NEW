#include "Puzzle.h"

Puzzle::Puzzle(std::string name, std::string clue, int pieceCount, GameObject* parent)
{

	m_puzzleName = name;
	m_clue = clue;

	m_pieces = std::vector<bool>(pieceCount, false);

	m_parent = parent;


}

bool Puzzle::hasBeenSolved()
{
	for (auto pieceFlag : m_pieces) {

		if (pieceFlag == false) {
			return false;
		}

	}

	return true;
}

void Puzzle::applyPuzzlePiece(std::string puzzlePieceId)
{




}


