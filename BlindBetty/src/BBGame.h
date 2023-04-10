#pragma once

#include "MobyDick.h"

class BBGame : public Game {

public:

	BBGame() = default;
	//~MRGame();

	static BBGame& instance();

	bool init(std::shared_ptr<ContactListener>, std::shared_ptr<ContactFilter>,
		std::shared_ptr<ComponentFactory>, std::shared_ptr<ActionFactory>, std::shared_ptr<ParticleEffectsFactory>,
		std::shared_ptr<CutSceneFactory>, std::shared_ptr<IMGuiFactory>, std::shared_ptr<TriggerFactory>, std::shared_ptr<ContextManager>,
		std::shared_ptr<EnumMap> enumMap, std::shared_ptr<ColorMap> colorMap) override;

private:

};


