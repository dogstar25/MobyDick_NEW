#include "EnvironmentComponent.h"
#include "../SceneManager.h"
#include "../game.h"

extern std::unique_ptr<Game> game;


EnvironmentComponent::EnvironmentComponent(Json::Value componentJSON, GameObject* parent) :
	Component(ComponentTypes::ENVIRONMENT_COMPONENT, parent)
{


}

void EnvironmentComponent::postInit()
{




}

void EnvironmentComponent::addEvent(std::shared_ptr<EnvironmentEvent> event)
{
	m_events.push_back(event);
}


void EnvironmentComponent::update()
{

	std::vector<std::shared_ptr<EnvironmentEvent>>  newEvents{};
	std::vector<std::shared_ptr<EnvironmentEvent>> allNewEvents{};

	auto it = m_events.begin();

	while (it != m_events.end()) {

		if (((*it)->hasExpired() && (*it)->status() == EventStatus::IN_PROGRESS) || (*it)->status() == EventStatus::COMPLETE) {

			(*it)->stop(parent());
			it = m_events.erase(it);

		}
		else {

			newEvents = (*it)->perform(parent());
			if (newEvents.empty() == false) {
				allNewEvents.append_range(newEvents);
			}
			++it;

		}

	}

	//If any newly created events came from the previous event-run, then add them to the back of the queue
	if (allNewEvents.empty() == false) {
		m_events.insert(m_events.end(), allNewEvents.begin(), allNewEvents.end());
	}

	//If there are no events left in this sequence, then move to the next sequence of events
	if (isEventDequeEmpty()) {

		if (m_currentSequence < m_sequenceCount) {

			_applyEventInstructions(m_currentSequence);

			m_currentSequence++;

		}
		else {
			m_currentSequence = 0;
			//_applyEventInstructions(m_currentSequence);
		}

	}


}

bool EnvironmentComponent::isEventDequeEmpty()
{
	return m_events.empty();
}

void EnvironmentComponent::startCycle()
{

	m_currentSequence = 0;
	m_sequenceCount = m_currentCycleInstructions.size();


}

void EnvironmentComponent::_applyEventInstructions(int sequence)
{


	for (const auto& instruction : m_currentCycleInstructions[sequence]) {

		auto event = game->environmentEventFactory()->create(instruction.id, instruction.param, instruction.durationTimer);
		m_events.push_back(event);

	}


}


