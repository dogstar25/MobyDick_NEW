#pragma once
#include "Action.h"

class InteractAction : public Action
{


public:

    InteractAction(Json::Value properties, GameObject* parent)
        :Action(properties, parent) {}

    void perform(GameObject* gameObject, SDL_Scancode keyScanCode) override;


};

