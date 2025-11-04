#include "SoundManager.h"

#include <fstream>
#include <format>

#include <json/json.h>

#include "GameConfig.h"
#include "game.h"
#include "Util.h"
#include "ResourceManager.h"
#include "../include/GameGlobals.h"

//extern std::unique_ptr<Game>game;


SoundManager::SoundManager()
{

}

SoundManager::~SoundManager()
{
	m_sfxChunks.clear();
	m_sfxMusic.clear();
}

SoundManager& SoundManager::instance()
{
	static SoundManager singletonInstance;
	return singletonInstance;

}

bool SoundManager::initSound()
{
	auto result = Mix_OpenAudio(48000, AUDIO_F32SYS, 2, 1024);
	if (result != 0) {

		SDL_Log("%s", Mix_GetError());
		return false;
	}

	auto volume = game->contextMananger()->getSoundVolume();
	Mix_Volume(-1, volume);
	
	loadSounds();
	allocateChannels();

	return true;
}

void SoundManager::update()
{

	auto volume = game->contextMananger()->getSoundVolume();
	setVolume( volume);

}


void SoundManager::loadSounds()
{

	std::string id, desc, filename;

	//Read file and stream it to a JSON object
	auto soundAssetResult = mobydick::ResourceManager::getJSON("sound/soundAssets.json");
	if (!soundAssetResult) {

		SDL_Log("SoundManager : Error loading sound asets");
		std::abort();

	}

	Json::Value soundAssetsJSON = soundAssetResult.value();

	//Store the sound affects sound items
	for (auto chunksItr : soundAssetsJSON["soundEffects"])
	{

		id = chunksItr["id"].asString();

		std::string soundAssetFile = "sound/" + chunksItr["filename"].asString();
		auto soundAssetResult = mobydick::ResourceManager::getSound(soundAssetFile);
		if (!soundAssetResult) {

			SDL_Log("%s", soundAssetResult.error().c_str());
			std::abort();

		}
		else {
			m_sfxChunks.emplace(id, soundAssetResult.value());
		}

		

	}

	//Store the sound music items
	for (auto musicItr : soundAssetsJSON["music"])
	{

		id = musicItr["id"].asString();

		std::string musicAssetFile = "sound/music/" + musicItr["filename"].asString();
		auto musicAssetResult = mobydick::ResourceManager::getMusic(musicAssetFile);
		if (!musicAssetResult) {

			SDL_Log("%s", musicAssetResult.error().c_str());
			std::abort();

		}
		else {
			m_sfxMusic.emplace(id, musicAssetResult.value());
		}
		

	}

}

void SoundManager::stopChannel(int channel)
{
	Mix_SetDistance(channel, 0);
	Mix_HaltChannel(channel);

}

void SoundManager::stopMusic()
{
	int channelPlayedOn = Mix_HaltMusic();;

}

int SoundManager::playSound(std::string id, int distanceMagnitude, bool loops )
{
	int channelPlayedOn{};
	int loopFlag{};
	if (loops) {
		loopFlag = -1;
	}
	
	//int availableChannel = -1;
	int availableChannel = Mix_GroupAvailable(-1);

	if (availableChannel != -1) {
		if (distanceMagnitude != 1) {
			Mix_SetDistance(availableChannel, distanceMagnitude);
		}

		channelPlayedOn = Mix_PlayChannel(availableChannel, m_sfxChunks[id], loopFlag);
	}
	else {
		SDL_Log("Negative sound channel for %s", id.c_str());
		//std::cout << "negative sound channel for " << id << std::endl;
		return -1;
	}


	return channelPlayedOn;

}

void SoundManager::playMusic(std::string id, int loopTimes)
{
	Mix_PlayMusic(m_sfxMusic[id], loopTimes);

}

void SoundManager::pauseSound()
{
	Mix_PauseMusic();
	Mix_Pause(-1);

}

void SoundManager::resumeSound()
{
	Mix_ResumeMusic();
	Mix_Resume(-1);

}

void SoundManager::setChannelDistance(int channel, int distance)
{

	Mix_SetDistance(channel, distance);

}

void SoundManager::allocateChannels()
{
	int soundChannels = GameConfig::instance().soundChannels();

	//Mix_AllocateChannels(soundChannels);
	int channels = Mix_AllocateChannels(soundChannels);

}

void SoundManager::setVolume(int volume)
{

	//Indifidual sound effects volume
	Mix_Volume(-1, volume);

	//music volume
	Mix_VolumeMusic(volume);

	//Total sound effects mix volumn - so things dont completely blast speakers when all sounds playing together
	//Mix_MasterVolume(int(volume - (volume * .20)));


}

