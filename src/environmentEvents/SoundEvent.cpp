#include "SoundEvent.h"
#include "../components/SoundComponent.h"

SoundEvent::SoundEvent(std::variant<std::string, EventLevel, float> param, Timer durationTimer):
	EnvironmentEvent(durationTimer)
{

	//Get the name of the soundLoop sound Id
	m_soundId = std::get<std::string>(param);




}

std::vector<std::shared_ptr<EnvironmentEvent>> SoundEvent::perform(GameObject* enviornmentGameObject)
{

	if (m_status == EventStatus::IDLE) {

		m_status = EventStatus::IN_PROGRESS;

		const auto& soundComponent = enviornmentGameObject->getComponent<SoundComponent>(ComponentTypes::SOUND_COMPONENT);
		m_soundChannel = soundComponent->playSound(m_soundId);
		
	}

	//empty
	return std::vector<std::shared_ptr<EnvironmentEvent>>();
	
}

void SoundEvent::stop(GameObject* enviornmentGameObject)
{

	m_status = EventStatus::COMPLETE;

}
