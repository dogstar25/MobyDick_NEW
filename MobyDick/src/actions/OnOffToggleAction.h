#pragma once
#include "Action.h"
#include "../BaseConstants.h"

class OnOffToggleAction : public Action
{

public:

    OnOffToggleAction(Json::Value properties, GameObject* parent)
        :Action(properties, parent) {}

    virtual void perform() override;


};

