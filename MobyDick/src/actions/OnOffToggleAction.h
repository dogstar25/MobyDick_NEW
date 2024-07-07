#pragma once
#include "Action.h"
#include "../BaseConstants.h"

class OnOffToggleAction : public Action
{

public:

    OnOffToggleAction(Json::Value properties)
        :Action(properties) {}

    virtual void perform(GameObject* gameObject) override;


};

