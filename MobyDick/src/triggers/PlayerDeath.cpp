#include "PlayerDeath.h"

#include "../Scene.h"
#include "../SceneManager.h"

bool PlayerDeath::hasMetCriteria()
{

	bool hasMet{ false };

	Scene& scene = SceneManager::instance().scenes().back();
	for (auto& gameObjects : scene.gameObjects())
	{

		for (int i = 0; i < gameObjects.size(); i++)
		{
			//assert(gameObject != nullptr && "GameObject is null");
			if (gameObjects[i]->id() == "GINA_64") {

				const auto& transformComponent = gameObjects[i]->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
				if (transformComponent->getCenterPosition().x > 400) {
					hasMet = true;
				}
				break;
			}

		}


	}

	return hasMet;
}


void PlayerDeath::execute()
{

	SDL_Event event;

	SceneAction* sceneAction = new SceneAction();
	sceneAction->actionCode = SCENE_ACTION_ADD;
	sceneAction->sceneId = "SCENE_PLAYER_DEATH";

	event.type = SDL_USEREVENT;
	event.user.data1 = sceneAction;
	SDL_PushEvent(&event);



	//instead of this how about adding a "GUI_PLAYERDEATH_PANEL" gameObject to the current scene


}
