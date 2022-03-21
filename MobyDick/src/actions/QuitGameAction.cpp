#include "QuitGameAction.h"


#include "../Scene.h"

QuitGameAction::QuitGameAction()
{

}

QuitGameAction::~QuitGameAction()
{

}

void QuitGameAction::perform()
{

	SDL_Event event;

	SceneAction* sceneAction = new SceneAction();
	sceneAction->actionCode = SCENE_ACTION_QUIT;
	sceneAction->actionId = "";

	event.type = SDL_USEREVENT;
	event.user.data1 = sceneAction;
	SDL_PushEvent(&event);

}
