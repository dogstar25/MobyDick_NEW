#include "PistolWeaponComponent.h"

#include "components/WeaponComponent.h"
#include "components/InventoryComponent.h"
#include "../GameConstants.h"
#include "GameObject.h"
#include "game.h"

extern std::unique_ptr<Game> game;



PistolWeaponComponent::PistolWeaponComponent(Json::Value definitionJSON)
	: WeaponComponent(definitionJSON)
{

}

void PistolWeaponComponent::checkLevelUp(const int scrapCount)
{

	if (scrapCount >= (m_currentLevel * m_levelUpIncrement)) {
		_levelUp();
		game->contextMananger()->setStatusItemValue(StatusItemId::PLAYER_WEAPON_LEVEL, m_currentLevel);
	}

	//Update the accrual amount used by things like the hud
	//Only if we are NOT at the maz level already
	if (m_currentLevel < m_weaponLevelDetails.size()) {
		m_currentLevelUpAccrual = scrapCount - (m_levelUpIncrement * (m_currentLevel - 1));
	}
	else {
		m_currentLevelUpAccrual = 0;
	}

	game->contextMananger()->setStatusItemValue(StatusItemId::PLAYER_WEAPON_ACCRUAL, m_currentLevelUpAccrual);

}
