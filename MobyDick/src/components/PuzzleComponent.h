#pragma once
#include "Component.h"
#include "../puzzles/Puzzle.h"
#include <optional>

class PuzzleComponent :  public Component
{

public:
	PuzzleComponent(Json::Value definitionJSON);
	~PuzzleComponent() = default;

	bool hasBeenSolved();
	std::optional<std::shared_ptr<Puzzle>> isApplicable(GameObject* puzzlePieceObject);
	std::map<std::string, std::shared_ptr<Puzzle>> puzzles() { return m_puzzles; }


	void update() override;
	void render();

private:

	std::map<std::string, std::shared_ptr<Puzzle>> m_puzzles{};

};

