#pragma once
#include "Action.h"

class InteractAction : public Action
{


public:

    InteractAction(Json::Value properties)
        :Action(properties) {}

    void perform(GameObject* gameObject, SDL_Scancode keyScanCode) override;


};

