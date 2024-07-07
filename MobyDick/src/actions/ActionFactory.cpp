#include "ActionFactory.h"
#include "DefaultMoveAction.h"
#include "DefaultRotateAction.h"
#include "DefaultOnHoverAction.h"
#include "DefaultOnHoverOutAction.h"
#include "PrimitiveMoveAction.h"
#include "DroneMoveAction.h"
#include "InteractAction.h"
#include "NoAction.h"
#include "ActorWarpAction.h"
#include "ToggleInventoryAction.h"
#include "CloseInventoryAction.h"
#include "ButtonCloseInventoryAction.h"
#include "ItemhHighlightApplyAction.h"
#include "ItemhHighlightRemoveAction.h"
#include "TakeItemAction.h"
#include "CloseShelfInventoryAction.h"
#include "OnOffToggleAction.h"


#include <iostream>


std::shared_ptr<Action> ActionFactory::create(std::string actionType, Json::Value properties)
{
    std::shared_ptr<Action> action{};

    if (actionType == "NoAction") {

        action =  std::make_shared<NoAction>(properties);
    }
    else if (actionType == "DefaultMove") {

        action = std::make_shared<DefaultMoveAction>(properties);
    }
    else if (actionType == "DefaultRotate") {

        action = std::make_shared<DefaultRotateAction>(properties);
    }
    else if (actionType == "DefaultHover") {

        action = std::make_shared<DefaultOnHoverAction>(properties);
    }
    else if (actionType == "DefaultHoverOut") {

        action = std::make_shared<DefaultOnHoverOutAction>(properties);
    }
    else if (actionType == "PrimitiveMove") {

        action = std::make_shared<PrimitiveMoveAction>(properties);
    }
    else if (actionType == "DroneMove") {

        action = std::make_shared<DroneMoveAction>(properties);
    }
    else if (actionType == "Interact") {

        action = std::make_shared<InteractAction>(properties);
    }
    else if (actionType == "COLL_Player_Wall") {

        action = std::make_shared<NoAction>(properties);
    }
    else if (actionType == "ActorWarpAction") {

        action = std::make_shared<ActorWarpAction>(properties);
    }
    else if (actionType == "ToggleInventoryAction") {

        action = std::make_shared<ToggleInventoryAction>(properties);
    }
    else if (actionType == "CloseInventoryAction") {

        action = std::make_shared<CloseInventoryAction>(properties);
    }
    else if (actionType == "ButtonCloseInventoryAction") {

        action = std::make_shared<ButtonCloseInventoryAction>(properties);
    }
    else if (actionType == "ItemhHighlightApplyAction") {

        action = std::make_shared<ItemhHighlightApplyAction>(properties);
    }
    else if (actionType == "ItemhHighlightRemoveAction") {

        action = std::make_shared<ItemhHighlightRemoveAction>(properties);
    }
    else if (actionType == "TakeItemAction") {

        action = std::make_shared<TakeItemAction>(properties);
    }
    else if (actionType == "CloseShelfInventoryAction") {

        action = std::make_shared<CloseShelfInventoryAction>(properties);
    }
    else if (actionType == "OnOffToggleAction") {

        action = std::make_shared<OnOffToggleAction>(properties);
    }

    

    

    else {
        std::cout << "No Action was Matched" << std::endl;
        action = std::make_shared<NoAction>(properties);
    }

    return action;
}
