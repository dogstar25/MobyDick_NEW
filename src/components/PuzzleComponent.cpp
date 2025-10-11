#include "PuzzleComponent.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

PuzzleComponent::PuzzleComponent(Json::Value componentJSON, GameObject* parent) :
	Component(ComponentTypes::PUZZLE_COMPONENT, parent)
{

	puzzle = game->puzzleFactory()->create(componentJSON, parent);

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


