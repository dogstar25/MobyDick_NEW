#include "PlayCutSceneAction.h"
#include "../Util.h"

void PlayCutSceneAction::perform(GameObject* gameObject)
{

	SDL_Event event;

	//Get the cut scene name from properties
	std::string cutScene = getActionProperty("cutSceneName").asString();


	//Create the event that will trigger the end of the scene to the sceneManager
	util::sendSceneEvent(SCENE_ACTION_DIRECT, cutScene);

}

