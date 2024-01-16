#pragma once
#include "Action.h"

class OnOffToggleAction : public Action
{

public:

    OnOffToggleAction() = default;

    virtual void perform(GameObject* gameObject) override;

private:

};

