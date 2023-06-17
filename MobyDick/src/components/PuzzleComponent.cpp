#include "PuzzleComponent.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

PuzzleComponent::PuzzleComponent(Json::Value componentJSON)
{
	m_componentType = ComponentTypes::PUZZLE_COMPONENT;

	//Build a Puzzle object for each puzzle
	for (Json::Value itrPuzzle : componentJSON["puzzles"])
	{

		std::string name = itrPuzzle["name"].asString();
		std::string clue = itrPuzzle["clue"].asString();
		const auto& puzzle = m_puzzles.emplace(name, game->puzzleFactory()->create(itrPuzzle["puzzleClass"].asString()));

		puzzle.first->second->setName(name);
		puzzle.first->second->setClue(clue);

		//add the puzzle piece items to the created puzzle
		for (Json::Value puzzlePiece : itrPuzzle["pieces"]) {

			puzzle.first->second->addPuzzlePiece(puzzlePiece["id"].asString());

		}

	}


}

void PuzzleComponent::update()
{

}

void PuzzleComponent::render()
{

}


bool PuzzleComponent::hasBeenSolved()
{
	//Have all of the puzzles been solved
	auto it = m_puzzles.begin();
	while(it != m_puzzles.end()) {

		if (it->second->hasBeenSolved() == false) {
			return false;
		}

		it++;
	}

	return true;
}

std::optional<std::shared_ptr<Puzzle>> PuzzleComponent::isApplicable(GameObject* puzzlePieceObject)
{
	if (m_puzzles.find(puzzlePieceObject->name()) != m_puzzles.end()) {

		return m_puzzles.at(puzzlePieceObject->name());
	}
	return std::nullopt;
}

