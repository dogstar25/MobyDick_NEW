#include "SoundLoopEvent.h"
#include "../components/SoundComponent.h"

SoundLoopEvent::SoundLoopEvent(std::variant<std::string, EventLevel, float> param, Timer durationTimer): 
	EnvironmentEvent(durationTimer)
{

	//Get the name of the soundLoop sound Id
	m_soundId = std::get<std::string>(param);




}

std::vector<std::shared_ptr<EnvironmentEvent>> SoundLoopEvent::perform(GameObject* enviornmentGameObject)
{

	if (m_status == EventStatus::IDLE) {

		m_status = EventStatus::IN_PROGRESS;
		m_durationTimer.reset();

		const auto& soundComponent = enviornmentGameObject->getComponent<SoundComponent>(ComponentTypes::SOUND_COMPONENT);
		m_soundChannel = soundComponent->playSound(m_soundId);
		
	}

	//empty
	return std::vector<std::shared_ptr<EnvironmentEvent>>();
	
}

void SoundLoopEvent::stop(GameObject* enviornmentGameObject)
{

	if (m_soundChannel.has_value()) {
		const auto& soundComponent = enviornmentGameObject->getComponent<SoundComponent>(ComponentTypes::SOUND_COMPONENT);
		soundComponent->stopSound(m_soundId);

	}

}
