#include "NavigationManager.h"
#include "LevelManager.h"


void NavigationManager::buildNavigationMapItem(GameObject* gameObject, Scene* scene)
{

	NavigationMapItem navigationMapItem{};

	//This is a normal wall
	if (gameObject->hasTrait(TraitTag::impasse) ||
		gameObject->hasTrait(TraitTag::conditional_impasse) ||
		gameObject->hasTrait(TraitTag::complex_impasse))
	{

		//Find this objects shared_ptr so we can store the weak_ptr of it here
		auto gameObjectSharedPtr = scene->getGameObject(gameObject->id()).value();
		auto angle = gameObjectSharedPtr->getAngleInDegrees();

		navigationMapItem.gameObject = gameObjectSharedPtr;

		//If this is impasse, then it will always be so set it now so that the 
		//the navigationComponent will work on the first pass
		if (gameObject->hasTrait(TraitTag::impasse)) {
			navigationMapItem.passable = false;
		}

		//Add the initial navigationMapItem to the collection
		m_navigationMap[(int)gameObject->getOriginalTilePosition().x][(int)gameObject->getOriginalTilePosition().y] = navigationMapItem;

		//Assert check that certain conditional na objects have to be a 0 or 90
		if (gameObject->hasTrait(TraitTag::conditional_impasse) ||
			gameObject->hasTrait(TraitTag::complex_impasse)) {

			assert((angle == 0 || angle == 90) && "A conditional_impasse object needs to be 0 or 90 degrees,(top to bottom or Left to right)");

		}

	}
	else {

		navigationMapItem.passable = true;
		m_navigationMap[(int)gameObject->getOriginalTilePosition().x][(int)gameObject->getOriginalTilePosition().y] = navigationMapItem;

	}

}

void NavigationManager::setNavigationMapArraySize(int width, int height)
{
	m_navigationMap.resize(width, std::vector<NavigationMapItem>(height));

}

void NavigationManager::updateNavigationMap()
{

	for (auto x = 0; x < m_navigationMap.size(); ++x)
	{
		for (auto y = 0; y < m_navigationMap[x].size(); ++y)
		{


			auto& navItem = m_navigationMap[x][y];

			//If this gameObject hasnt been deleted elsewhere somehow the get the gameObject representing 
			// this navigation map grid location
			if (navItem.gameObject.has_value() && navItem.gameObject->expired() == false) {

				const auto& gameObject = navItem.gameObject->lock();

				//Get the gameobject and its width in tiles
				int widthInTiles = int(std::round(gameObject->getSize().x / LevelManager::instance().m_tileWidth));

				//Plain impassable
				if (gameObject->hasTrait(TraitTag::impasse)) {
					if (m_navigationMap[x][y].passable != false) {
						m_navigationMapChanged = true;
					}
					m_navigationMap[x][y].passable = false;
				}
				else if (gameObject->hasTrait(TraitTag::conditional_impasse)) {


					//if we are at 0 angle then we need to set the next "widthInTiles" tiles horizontally to impassable
					if ((int)gameObject->getAngleInDegrees() == 0) {

						if (gameObject->physicsDisabled() == true) {

							//set the next tiles to the right as no-impasse
							for (auto i = 0; i < widthInTiles; i++) {
								if ((x + i) < LevelManager::instance().m_width - 1) {

									if (m_navigationMap[x][y].passable != true) {
										m_navigationMapChanged = true;
									}

									m_navigationMap[x + i][y].passable = true;
								}
							}
						}
						else {

							//set the next tiles to the right as no-impasse
							for (auto i = 0; i < widthInTiles; i++) {
								if ((x + i) < LevelManager::instance().m_width - 1) {

									if (m_navigationMap[x][y].passable != false) {
										m_navigationMapChanged = true;
									}

									m_navigationMap[x + i][y].passable = false;
								}
							}

						}

					}
					//if we are at 90 angle then we need to set the next "widthInTiles" tiles vertically to impassable
					else if ((int)gameObject->getAngleInDegrees() == 90) {

						if (gameObject->physicsDisabled() == true) {

							//set the next tiles to the right as no-impasse
							for (auto i = 0; i < widthInTiles; i++) {
								if ((y + i) < LevelManager::instance().m_height - 1) {

									if (m_navigationMap[x][y].passable != true) {
										m_navigationMapChanged = true;
									}

									m_navigationMap[x][y + i].passable = true;
								}
							}
						}
						else {

							//set the next tiles to the right as no-impasse
							for (auto i = 0; i < widthInTiles; i++) {
								if ((y + i) < LevelManager::instance().m_height - 1) {

									if (m_navigationMap[x][y].passable != false) {
										m_navigationMapChanged = true;
									}

									m_navigationMap[x][y + i].passable = false;
								}
							}

						}

					}

				}
				//This is a composite object that needs a different type of check for passable			
				else if (gameObject->hasTrait(TraitTag::complex_impasse)) {


					//if we are at 0 angle then we need to set the next "widthInTiles" tiles to impassable
					if ((int)navItem.gameObject->lock()->getAngleInDegrees() == 0) {

						//Has the composite object been destroyed
						if (gameObject->isCompositeEmpty() == true) {

							//Set the next "widthInTiles" tiles vertically to passable
							for (auto i = 0; i < widthInTiles; i++) {
								if ((x + i) < LevelManager::instance().m_width - 1) {

									if (m_navigationMap[x][y].passable != true) {
										m_navigationMapChanged = true;
									}

									m_navigationMap[x + i][y].passable = true;
								}
							}
						}
						else {

							//Set the next "widthInTiles" tiles vertically to impassable
							for (auto i = 0; i < widthInTiles; i++) {
								if ((x + i) < LevelManager::instance().m_width - 1) {

									if (m_navigationMap[x][y].passable != false) {
										m_navigationMapChanged = true;
									}

									m_navigationMap[x + i][y].passable = false;
								}
							}

						}

					}
					//if we are at 0 angle then we need to set the next "widthInTiles" tiles to impassable
					else if (gameObject->getAngleInDegrees() == 90) {

						if (gameObject->isCompositeEmpty() == true) {

							//set the next tiles to the right as no-impasse
							for (auto i = 0; i < widthInTiles; i++) {
								if ((y + i) < LevelManager::instance().m_height - 1) {

									if (m_navigationMap[x][y].passable != true) {
										m_navigationMapChanged = true;
									}

									m_navigationMap[x][y + i].passable = true;
								}
							}
						}
						else {

							//set the next tiles to the right as no-impasse
							for (auto i = 0; i < widthInTiles; i++) {
								if ((y + i) < LevelManager::instance().m_height - 1) {

									if (m_navigationMap[x][y].passable != false) {
										m_navigationMapChanged = true;
									}

									m_navigationMap[x][y + i].passable = false;
								}
							}

						}

					}

				}
			}
		}
	}

	return;

}

