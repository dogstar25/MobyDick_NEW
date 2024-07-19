#include "Action.h"

Action::~Action()
{

}

Action::Action(Json::Value properties, GameObject* parent)
	:m_properties(properties), m_parent(parent)
{
	
	m_status = ProgressionStatus::COMPLETE;
}
