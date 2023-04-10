#pragma once

#include "MobyDick.h"


class BBParticleEffectsFactory : public ParticleEffectsFactory{



public:

	virtual ParticleEffect create(std::string particleType) override;


private:




};
