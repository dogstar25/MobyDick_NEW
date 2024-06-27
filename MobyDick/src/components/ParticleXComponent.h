#ifndef PARTICLE_X_COMPONENT_H
#define PARTICLE_X_COMPONENT_H

#include <string>
#include <memory>
#include <chrono>
#include <vector>

#include <json/json.h>

#include "Component.h"
#include "../Util.h"
#include "../BaseConstants.h"
#include "../particleEffects/BaseParticleEffects.h"
#include "../Timer.h"

class GameObject;

class ParticleXComponent : public Component
{
public:
	ParticleXComponent(Json::Value componentJSON, GameObject* parent);
	~ParticleXComponent();

	void update() override;
	void setType(int type) { m_type = type; }
	void setEmissionInterval(float emissionInterval);
	void addParticleEffect(ParticleEffect particleEffect);

private:
	int m_type{ParticleEmitterType::ONETIME};
	std::vector<ParticleEffect> m_particleEffects;
	std::chrono::duration<float, std::milli> m_emissionInterval{ std::chrono::duration<float>(0.F) };
	Timer m_emissionTimer{};
	GameLayer m_emissionLayer{ GameLayer::MAIN };

};






#endif

