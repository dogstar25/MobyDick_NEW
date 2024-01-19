#include "PuzzleComponent.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

PuzzleComponent::PuzzleComponent(Json::Value componentJSON)
{
	m_componentType = ComponentTypes::PUZZLE_COMPONENT;

	puzzle = game->puzzleFactory()->create(componentJSON);


}

void PuzzleComponent::update()
{

}

void PuzzleComponent::render()
{

}


bool PuzzleComponent::hasBeenSolved()
{
	return puzzle->hasBeenSolved();
}

bool PuzzleComponent::isApplicable(GameObject* puzzlePieceObject)
{

	return puzzle->isPuzzlePieceApplicable(puzzlePieceObject);
	

}


