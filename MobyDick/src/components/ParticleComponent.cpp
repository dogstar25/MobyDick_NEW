#include "ParticleComponent.h"


#include "../game.h"
#include "../EnumMap.h"

extern std::unique_ptr<Game> game;

ParticleComponent::ParticleComponent(Json::Value componentJSON, GameObject* parent) :
	Component(ComponentTypes::PARTICLE_COMPONENT, parent)
{

	m_maxParticles = componentJSON["maxParticles"].asInt();
	for (int i = 0; i < m_maxParticles; i++) {

		m_particles.emplace_back(Particle());

	}

	auto emissionInterval = componentJSON["emissionInterval"].asFloat();
	m_emissionTimer = Timer(emissionInterval, true);

	//Add any effects that may be pre-built into the particle emitter
	for (Json::Value itrEffect : componentJSON["effects"])
	{
		auto effectId = itrEffect.asString();

		ParticleEffect effect = game->particleEffectsFactory()->create(effectId);
		addParticleEffect(effect);
	}

	if (componentJSON.isMember("type")) {
		m_type = game->enumMap()->toEnum(componentJSON["type"].asString());
	}

}

ParticleComponent::~ParticleComponent()
{

}

void ParticleComponent::render()
{

	//creating the particles runs asynchronously, so only render them if that is complete
	if (!m_creationComplete) {
		return;
	}
	const auto& renderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

	std::lock_guard<std::mutex> lock(m_particlesMutex);
	for (auto& particle : m_particles) {

		if (particle.isActive == true) {

			SDL_FRect positionRect = { 
				particle.position.x, particle.position.y, 
				particle.size.x, particle.size.y
			};

			SDL_FRect destRect = renderComponent->getRenderDestRect(positionRect);

			//adjust angle to match what the drawsprite wants
			float angle = particle.angle + 90;

			game->renderer()->drawSprite(parent()->layer(), destRect, particle.color, particle.texture, &particle.texture->textureAtlasQuad, 
				angle, false, SDL_Color{}, RenderBlendMode::ADD);

		}
	}

}

void ParticleComponent::update()
{
	bool activeParticleFound = false;

	//First update the position,lifetime,etc of all active particles
	//for (auto& particle : m_particles) {

	//	if (particle.isActive == true) {

	//		activeParticleFound = true;

	//		if (particle.lifetimeTimer.hasMetTargetDuration()) {

	//			particle.isAvailable = true;
	//			particle.isActive = false;
	//		}
	//		else {
	//		}

	//		float timeFactor{ GameConfig::instance().gameLoopStep() };
	//		if (SceneManager::instance().gameTimer().timeRemaining().count() > 0) {
	//			timeFactor = SceneManager::instance().gameTimer().timeRemaining().count();
	//		}
	//		particle.position.x += particle.velocity.x * timeFactor;
	//		particle.position.y += particle.velocity.y * timeFactor;

	//		if (particle.alphaFade == true) {
	//			Uint8 alpha = int(particle.originalALpha * (100 - particle.lifetimeTimer.percentTargetMet()));
	//			particle.color.a = alpha;
	//		}
	//	}

	//}

	//Now emit more particles IF,
	//The emission interval timer has been met AND
	//this is either a continuous emitter or a onetime emitter that hasnt fired yet

		if (m_creationComplete) {
			m_creationComplete = false;  // Reset completion flag
			m_creationThread = std::thread(&ParticleComponent::createParticles, this);
			m_creationThread.detach();  // Run asynchronously without waiting
		}


	//If this is a one-time emission and all particles that were emitted are now inactive, 
	// then mark this emitter object to be removed
	if (m_type == ParticleEmitterType::ONETIME && activeParticleFound == false) {
		parent()->setRemoveFromWorld(true);
	}

}


void ParticleComponent::addParticleEffect(ParticleEffect particleEffect)
{
	m_particleEffects.push_back(particleEffect);
}

void ParticleComponent::createParticles()
{ 
	
	std::lock_guard<std::mutex> lock(m_particlesMutex);

	bool activeParticleFound{};

	for (auto& particle : m_particles) {

		if (particle.isActive == true) {

			activeParticleFound = true;

			if (particle.lifetimeTimer.hasMetTargetDuration()) {

				particle.isAvailable = true;
				particle.isActive = false;
			}
			else {
			}

			float timeFactor{ GameConfig::instance().gameLoopStep() };
			if (SceneManager::instance().gameTimer().timeRemaining().count() > 0) {
				timeFactor = SceneManager::instance().gameTimer().timeRemaining().count();
			}
			particle.position.x += particle.velocity.x * timeFactor;
			particle.position.y += particle.velocity.y * timeFactor;

			if (particle.alphaFade == true) {
				Uint8 alpha = int(particle.originalALpha * (100 - particle.lifetimeTimer.percentTargetMet()));
				particle.color.a = alpha;
			}
		}

	}

	if ((m_type == ParticleEmitterType::CONTINUOUS && m_emissionTimer.hasMetTargetDuration() == true) ||
		(m_type == ParticleEmitterType::ONETIME && m_oneTimeEmitted == false)) {


		for (auto& effect : m_particleEffects) {

			//Get the texture by retrieving the effects pooled object and grabbing its texture
			Texture* texture = parent()->parentScene()->objectPoolManager().getPoolObjectTexture(effect.poolId).get();

			//If the particle count min and max are different, then generate a random count
			//that is between min and max , otherwise just use the max
			auto particleCount = util::generateRandomNumber(effect.particleSpawnCountMin, effect.particleSpawnCountMax);

			auto parentTransformComponent = parent()->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);

			for (int i = 0; i < particleCount; i++)
			{

				const auto& particle = getAvailableParticle();

				if (particle) {

					m_oneTimeEmitted = true;
					activeParticleFound = true;

					particle.value()->isAvailable = false;
					particle.value()->isActive = true;

					//Set the texture
					particle.value()->texture = texture;

					//Alpha Fade
					particle.value()->alphaFade = effect.alphaFade;

					//Set the color of the particle. Randomize the color values if they are different
					particle.value()->color = util::generateRandomColor(effect.colorRangeBegin, effect.colorRangeEnd);
					particle.value()->originalALpha = particle.value()->color.a;

					//Size
					if (effect.particleSizeMin.has_value()) {
						float size = util::generateRandomNumber(effect.particleSizeMin.value(), effect.particleSizeMax.value());
						particle.value()->size = { size, size };
					}
					else if (effect.particleSizeMinWidth.has_value()) {
						float width = util::generateRandomNumber(effect.particleSizeMinWidth.value(), effect.particleSizeMaxWidth.value());
						float height = util::generateRandomNumber(effect.particleSizeMinHeight.value(), effect.particleSizeMaxHeight.value());

						particle.value()->size = { width, height };
					}

					//Set the particles lifetime
					auto particleLifetime = util::generateRandomNumber(effect.lifetimeMin, effect.lifetimeMax);
					particle.value()->lifetimeTimer = Timer(particleLifetime);

					//Calculate the emit angle/direction that the particle will travel in
					//If this is a onetime emission, make each particle's angle symetric with the whole.
					//Otherwise, make each one random, but still within the angle range
					float emitAngle = 0;
					if (m_type == ParticleEmitterType::ONETIME) {
						auto angleRange = effect.angleMax - effect.angleMin;
						emitAngle = ((float)i / (float)particleCount) * angleRange;
					}
					else {
						emitAngle = util::generateRandomNumber(effect.angleMin, effect.angleMax);
					}

					particle.value()->angle = emitAngle;

					//emitAngle += effect.angleMin;
					emitAngle = util::degreesToRadians(emitAngle);

					//Calculate velocity vector
					auto force = util::generateRandomNumber(effect.forceMin * PARTICLE_EMITTER_FORCE_ADJ, effect.forceMax * PARTICLE_EMITTER_FORCE_ADJ);

					//Adjust force by a factor so that is closer matches what box2d does and we can use the same
					//particle effect objects easier
					particle.value()->velocity.x = cos(emitAngle) * (force);// *PARTICLE_EMITTER_FORCE_ADJ);
					particle.value()->velocity.y = sin(emitAngle) * (force);// *PARTICLE_EMITTER_FORCE_ADJ);

					//Position - If zero was passed in then use the location of the gameObject
					//that this ParticleComponent belongs to
					b2Vec2 positionVector = {};
					if (effect.originMin.Length() > 0 || effect.originMax.Length() > 0) {

						positionVector.x = util::generateRandomNumber(effect.originMin.x, effect.originMax.x);
						positionVector.y = util::generateRandomNumber(effect.originMin.y, effect.originMax.y);

					}
					else {

						positionVector = parentTransformComponent->position();
					}

					particle.value()->position.x = positionVector.x;
					particle.value()->position.y = positionVector.y;

				}

			}

		}
	}
	
	m_creationComplete = true;

}

std::optional<Particle*> ParticleComponent::getAvailableParticle()
{
	std::optional<Particle*> availableParticle = std::nullopt;

	for (auto& particle : m_particles) {

		if (particle.isAvailable) {

			availableParticle = &particle;
			break;
		}

	}

	return availableParticle;
}

void ParticleComponent::setParticleEffectSpawnCount(std::string effectName, int minSpawnCount, int maxSpawnCount)
{

	for (auto& effect : m_particleEffects) {

		if (effect.name == effectName) {

			effect.particleSpawnCountMin = minSpawnCount;
			effect.particleSpawnCountMax = maxSpawnCount;

		}

	}

}

void ParticleComponent::setParticleEffectForce(std::string effectName, float minEmitForce, float maxEmitForce)
{

	for (auto& effect : m_particleEffects) {

		if (effect.name == effectName) {

			effect.forceMin = minEmitForce;
			effect.forceMin = maxEmitForce;

		}

	}

}

void ParticleComponent::setParticleEffectEmitAngle(std::string effectName, float minEmitAngle, float maxEmitAngle)
{

	for (auto& effect : m_particleEffects) {

		if (effect.name == effectName) {

			effect.angleMin = minEmitAngle;
			effect.angleMax = maxEmitAngle;

		}

	}

}

