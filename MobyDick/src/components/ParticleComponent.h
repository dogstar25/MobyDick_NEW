#ifndef PARTICLE_COMPONENT_H
#define PARTICLE_COMPONENT_H

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <optional>
#include <thread>
#include <mutex>
#include <atomic>


#include <json/json.h>

#include "../texture.h"
#include "Component.h"
#include "../Util.h"
#include "../BaseConstants.h"
#include "../particleEffects/BaseParticleEffects.h"
#include "../Timer.h"

class GameObject;

struct Particle
{
	SDL_FPoint position{ 0,0 };
	SDL_FPoint size{ 1,1 };
	float speed { 1 };
	float angle{ 0 };
	SDL_FPoint velocity{ 0 };
	bool isAvailable { true };	
	bool isActive{ false };
	Texture* texture;
	SDL_Color color{ 255,255,255,255 };
	bool alphaFade{};
	Uint8 originalALpha{};
	Timer lifetimeTimer{};

};


class ParticleComponent : public Component
{
public:
	ParticleComponent(Json::Value componentJSON, GameObject* parent);
	~ParticleComponent();

	void update() override;
	void render();
	void setType(int type) { m_type = type; }
	void setEmissionInterval(float emissionIterval) { m_emissionInterval = std::chrono::duration<float>(emissionIterval); }
	void addParticleEffect(ParticleEffect particleEffect);
	std::optional<Particle*> getAvailableParticle();
	void setParticleEffectSpawnCount(std::string effectName, int minSpawnCount, int maxSpawnCount);
	void setParticleEffectEmitAngle(std::string effectName, float minEmitAngle, float maxEmitAngle);
	void setParticleEffectForce(std::string effectName, float minEmitForce, float maxEmitForce);

private:

	int m_type{ParticleEmitterType::ONETIME};
	bool m_oneTimeEmitted{ false };
	std::vector<ParticleEffect> m_particleEffects;
	std::chrono::duration<float, std::milli> m_emissionInterval{ std::chrono::duration<float>(0.f) };
	std::vector<Particle> m_particles;
	int m_maxParticles{};

	Timer m_emissionTimer{};

	void createParticles();

	std::atomic<bool> m_creationComplete{ true };
	std::thread m_creationThread;
	std::mutex m_particlesMutex;

};






#endif

