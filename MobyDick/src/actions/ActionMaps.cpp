#include "ActionMaps.h"
#include "ActorMoveAction.h"
#include "ActorRotateAction.h"
#include "ActorUseAction.h"
#include "ContinueGameClickAction.h"
#include "QuitGameAction.h"
#include "DefaultOnHoverAction.h"
#include "DefaultOnHoverOutAction.h"
#include "DefaultMoveAction.h"
#include "DefaultRotateAction.h"
#include "NoAction.h"

ActionMaps& ActionMaps::instance()
{
	static ActionMaps singletonInstance;
	return singletonInstance;
}


ActionMaps::ActionMaps()
{
	m_actionMap["NoAction"] = std::make_shared<NoAction>();
	m_actionMap["DefaultMove"] = std::make_shared<DefaultMoveAction>();
	m_actionMap["DefaultRotate"] = std::make_shared<DefaultRotateAction>();
	m_actionMap["DefaultHover"] = std::make_shared<DefaultOnHoverAction>();
	m_actionMap["DefaultHoverOut"] = std::make_shared<DefaultOnHoverOutAction>();

	m_actionMap["ActorMove"] = std::make_shared<ActorMoveAction>();
	m_actionMap["ActorRotate"] = std::make_shared<ActorRotateAction>();
	m_actionMap["ActorUse"] = std::make_shared<ActorUseAction>();

	m_actionMap["ContinueClick"] = std::make_shared<ContinueGameClickAction>();
	m_actionMap["QuitClick"] = std::make_shared<QuitGameAction>();

}


ActionMaps::~ActionMaps()
{

}