#pragma once
#include <string>
#include <map>
#include "../GameObject.h"

struct PuzzlePiece {

	std::string id{};
	bool isSolved{ false };
};

class Puzzle
{

public:

	Puzzle() = default;
	Puzzle(std::string name, std::string clue, int pieceCount, GameObject* parent);

	bool hasBeenSolved();
	virtual void applyPuzzlePiece(std::string puzzlePieceId);
	virtual bool isPuzzlePieceApplicable(GameObject* puzzlePieceObject) = 0;
	virtual void applyPuzzlePiece(GameObject* puzzleObject, GameObject* puzzlePieceObject) = 0;

	std::string name() { return m_puzzleName; }
	void setName(std::string name) { m_puzzleName = name; }
	std::string clue() { return m_clue; }
	void setClue(std::string clue) { m_clue = clue; }

	std::vector<bool>& pieces() { return m_pieces; }
	void setPiece(int index, bool solvedFlag) { m_pieces[index] = solvedFlag; }
	GameObject* parent() { return m_parent; }

protected:

	
	//GameObject that owns this puzzle
	GameObject* m_parent{};

	std::string m_puzzleName;
	std::string m_clue{};

	std::vector<bool>m_pieces{};

public:

	
};

/*
How a Puzzle Works

the isApplicable function is called by something like the contactHandler when 2 objects
tagged as 'puzzle' and 'puzzle_piece contact each other. isApplicable is given the puzzle_piece
object as argument and determines if it is for this particular puzzle and activates 
the isSolved to true

*/
