#include "SoundComponent.h"
#include "../SoundManager.h"
#include "../LevelManager.h"
#include "../IMGui/IMGuiUtil.h"
#include <memory.h>

#include "../EnumMap.h"
#include "../game.h"


extern std::unique_ptr<Game> game;

SoundComponent::SoundComponent(Json::Value componentJSON, GameObject* parent) :
	Component(ComponentTypes::SOUND_COMPONENT, parent)
{
	for (Json::Value itr: componentJSON["sounds"])
	{
		SoundItem soundItem{};
		soundItem.id = itr["id"].asString();
		soundItem.soundAssetId = itr["soundAssetId"].asString();
		soundItem.isDistanceSensitive = itr["distanceSensitive"].asBool();
		soundItem.isContinuous = itr["continuous"].asBool();
		soundItem.soundRange = itr["soundRange"].asInt();
		soundItem.lineOfSightAdjustment = itr["lineOfSightAdjustment"].asInt();
		m_sounds.emplace(soundItem.id, soundItem);

	}

}

SoundComponent::~SoundComponent()
{

	stopSounds();
	m_sounds.clear();

}
std::optional<SoundItem> SoundComponent::getSound(std::string soundId)
{
	std::optional<SoundItem> foundSound{};

	if (m_sounds.find(soundId) != m_sounds.end()) {

		foundSound = m_sounds[soundId];

	}

	return foundSound;
}

void SoundComponent::addSound(std::string id, std::string soundAssetId, bool isDistanceSensitive, bool isContinuous, int soundRange)
{

	SoundItem soundItem{};
	soundItem.id = id;
	soundItem.soundAssetId = soundAssetId;
	soundItem.isDistanceSensitive = isDistanceSensitive;
	soundItem.isContinuous = isContinuous;
	soundItem.soundRange = soundRange;

	m_sounds.emplace(soundItem.id, soundItem);


}

void SoundComponent::update()
{

	//Loop through all of the sounds and for ones that are continuous and distance sensitive, 
	//update the sound channel that its on with the distance from this object to the player
	auto itr = m_sounds.begin();
	while (itr != m_sounds.end()) {

		if (itr->second.isDistanceSensitive == true) {

			const auto player = parent()->parentScene()->player();
			if (itr->second.soundChannel.has_value()) {
				const auto playerPosition = parent()->parentScene()->player()->getCenterPosition();
				const auto parentPosition = parent()->getCenterPosition();
				int soundDistanceAdjustment = _calculateSoundDistanceAdjustment(playerPosition, parentPosition, itr->second.soundRange, itr->second.lineOfSightAdjustment);

				SoundManager::instance().setChannelDistance(itr->second.soundChannel.value(), soundDistanceAdjustment);
			}
		}

		++itr;

	}



}

void SoundComponent::postInit()
{

	//Loop though and find all sounds that are continuous and start them up and save their channel
	auto itr = m_sounds.begin();
	while (itr != m_sounds.end()) {

		if (itr->second.isContinuous == true) {

			int channel = SoundManager::instance().playSound(itr->second.soundAssetId, 255, true);
			itr->second.soundChannel = channel;
			SoundManager::instance().setChannelDistance(itr->second.soundChannel.value(), 255);

		}

		++itr;
	}


}

void SoundComponent::stopSounds()
{

	//Loop though and find all sounds that are continuous and stop them from playing to free up the channel
	auto itr = m_sounds.begin();
	while (itr != m_sounds.end()) {

		if (itr->second.isContinuous == true) {

			stopSound(itr->second.id);

		}

		++itr;
	}

}

void SoundComponent::stopSound(std::string soundId)
{

	if (m_sounds.find(soundId) != m_sounds.end()) {

		if (m_sounds.at(soundId).soundChannel.has_value()) {
			SoundManager::instance().stopChannel(m_sounds.at(soundId).soundChannel.value());

			//Set this sounds channel to nothing because it has been freed and could have been grabbed by another sound
			m_sounds.at(soundId).soundChannel = std::nullopt;
		}
	}

}


//void SoundComponent::setParent(GameObject* gameObject)
//{
//	//Call base component setParent
//	Component::setParent(gameObject);
//
//}

int SoundComponent::playSound(std::string soundId)
{
	int channel{};
	int soundDistanceMagnitude{1};

	if (m_sounds.find(soundId) != m_sounds.end()) {

		if (m_sounds.at(soundId).isDistanceSensitive) {

			const auto playerPosition = parent()->parentScene()->player()->getCenterPosition();
			const auto parentPosition = parent()->getCenterPosition();

			soundDistanceMagnitude = _calculateSoundDistanceAdjustment(playerPosition, parentPosition, (m_sounds.at(soundId).soundRange),
				(m_sounds.at(soundId).lineOfSightAdjustment));
		}

		//play
		channel = SoundManager::instance().playSound(m_sounds.at(soundId).soundAssetId, soundDistanceMagnitude, m_sounds.at(soundId).isContinuous);
		m_sounds.at(soundId).soundChannel = channel;
	}

	return channel;

}

int SoundComponent::_calculateSoundDistanceAdjustment(SDL_FPoint playerPosition, SDL_FPoint parentPosition, int soundRange, int lineOfSightAdjustment)
{

	float playerToSoundSourceDistance = util::calculateDistance(playerPosition, parentPosition);

	int soundMagnitude = int((playerToSoundSourceDistance / soundRange) * (float)255.0);
	soundMagnitude = std::min((int)soundMagnitude, 255);

	//If we have a value for lineOfSightAdjustment, then we need to see if there's any object tagged as barriers between
	//the player and the sound source and adjust sound if there are
	if (lineOfSightAdjustment > 0) {

		soundMagnitude = _adjustForLineOfSight(soundMagnitude, lineOfSightAdjustment);
	}


	return soundMagnitude;
}

int SoundComponent::_adjustForLineOfSight(int currentSoundMagnitude, int lineOfSightAdjustment)
{

	const auto& player = parent()->parentScene()->player();
	bool applyLightOfSightAdjustment{};
	int newSoundMagnitude{ currentSoundMagnitude };

	b2Vec2 begin = { parent()->getCenterPosition().x, parent()->getCenterPosition().y };
	b2Vec2 end = { player->getCenterPosition().x, player->getCenterPosition().y};

	util::toBox2dPoint(begin);
	util::toBox2dPoint(end);


	if (util::hasLineOfSight(parent(), player.get(), parent()->parentScene()->physicsWorld() )) {

		applyLightOfSightAdjustment = true;

	}

	if (applyLightOfSightAdjustment) {

		if (currentSoundMagnitude < 255) {

			newSoundMagnitude = std::max(255 - lineOfSightAdjustment, currentSoundMagnitude);
		}
	}


	return newSoundMagnitude;
}




