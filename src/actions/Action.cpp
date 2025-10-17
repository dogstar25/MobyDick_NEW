#include "Action.h"

Action::~Action()
{

}

Action::Action(Json::Value properties, GameObject* parent)
	:m_properties(properties), m_parent(parent)
{
	
	m_status = ProgressionStatus::COMPLETE;
}

Json::Value Action::getActionProperty(std::string property)
{
	for (Json::Value prop : m_properties) {

		if (prop.isMember(property)) {

			return prop[property];
		}

	}

	return Json::Value{};
}

Json::Value Action::hasActionProperty(std::string property)
{
	for (Json::Value prop : m_properties) {

		if (prop.isMember(property)) {

			return prop[property];
		}

	}

	return Json::Value{};
}

