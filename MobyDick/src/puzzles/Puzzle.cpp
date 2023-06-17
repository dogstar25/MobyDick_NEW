#include "Puzzle.h"

void Puzzle::applyPuzzlePiece(std::string puzzlePieceId)
{

	if (m_pieces.find(puzzlePieceId) != m_pieces.end()) {

		m_pieces[puzzlePieceId].isSolved = true;

	}

}

void Puzzle::addPuzzlePiece(std::string pieceId) 
{

	PuzzlePiece puzzlePiece;
	puzzlePiece.id = pieceId;
	puzzlePiece.isSolved = false;
	m_pieces[pieceId] = puzzlePiece;
}

