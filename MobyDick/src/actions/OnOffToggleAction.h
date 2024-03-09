#pragma once
#include "Action.h"
#include "../BaseConstants.h"

class OnOffToggleAction : public Action
{

public:

    OnOffToggleAction() = default;

    virtual void perform(GameObject* gameObject) override;


};

