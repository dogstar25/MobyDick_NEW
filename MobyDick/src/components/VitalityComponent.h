#ifndef VITALITY_COMPONENT_H
#define VITALITY_COMPONENT_H

#include <chrono>
#include <array>

#include "Component.h"
#include "../Timer.h"

class GameObject;

struct LevelDefinition
{
	short levelNum;
	short resistance;
	SDL_Color color;
};

class VitalityComponent :  public Component
{
public:
	VitalityComponent();
	VitalityComponent(Json::Value componentJSON, GameObject* parent);
	~VitalityComponent();

	void update() override;
	void render();
	bool testResistance(float force);

	float resistance() { return m_resistance; }
	void setResistance(float resistance) { m_resistance = resistance; }
	float attackPower() {	return m_attackPower;	}
	void setAttackPower(float attackPower) { m_attackPower = attackPower; }
	float speed() {	return m_speed; }
	void setSpeed(float speed) { m_speed = speed; }
	void setHealth(float health) { m_health = health; }
	float health() { return m_health; }
	float rotateSpeed() { return m_rotationSpeed; }
	void setLevelCap(int levelCap);

	bool isLifetimeAlphaFade() { return m_isLifetimeAlphaFade; }
	void setIsLifetimeAlphaFade(bool isLifetimeAlphaFade) { m_isLifetimeAlphaFade = isLifetimeAlphaFade; }

	void setLifetimeTimer(float lifetime);
	void resetLifetime();
	void reset();

	bool inflictDamage(float damage);
	bool addHealth(float damage);
	bool isDead();


private:
	float m_speed{ 0 };
	float m_rotationSpeed{};
	float m_attackPower{ 0 };
	float m_health{ 0 };
	float m_maxHealth{ 0 };
	float m_resistance{ 0 };
	bool m_hideWhenBroken{};
	bool m_isBroken{ false };

	//Regenerative
	bool	m_isRegenerative{ false };
	float	m_regenSpeed{ 0 };
	int		m_currentLevel{ 0 };
	int		m_maxLevels{ 0 };
	int     m_levelCap{ 1 };
	std::array<LevelDefinition, MAX_VITALITY_LEVELS> m_regenLevels = {};

	bool m_isLifetimeAlphaFade{ true };
	float m_lifetime{};
	float m_OriginalLifetime{};

	Timer m_lifetimeTimer{};
	Timer m_regenTimer{};

	void _levelUp();
	void _updateFiniteLifetime();
	void _updateRegeneration();
	void _breaK();
	void _restore();



	float* IG_Speed() { return &m_speed; }

};

#endif

