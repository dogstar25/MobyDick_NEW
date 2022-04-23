#pragma once
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class StatusItem
{

public:
	StatusItem(float value, float maxValue) :
		m_value(value),
		m_originalValue(value),
		m_maximumValue(maxValue) {}
	StatusItem() = default;
	void adjust(float adjustValue);
	float value() { return m_value; }
	void setValue(float newValue) { m_value = newValue; }

	void reset();

private:
	float m_value;
	float m_originalValue;
	float m_maximumValue;

};

const std::string GAME_FILENAME = "../game.dat";

struct UserSettings {

	int soundLevel{};
	int mouseSensitivity{};

};

class ContextManager
{
public:
	ContextManager();
	~ContextManager() = default;

	std::map<std::string, StatusItem> statusValueMap() { return m_statusValueMap; }
	float getValue(std::string valudId);
	void adjustValue(std::string valudId, float adjustmentValue);
	void setValue(std::string valudId, float newValue);

	void setMouseSensitivity(int mouseSensitivity);
	void setSoundVolume(int volume);

	virtual void initMappings();

	//These are the 1 - 100 GUI slider config values
	int getMouseSensitivityForGui();
	int getSoundVolumeForGui();

	//These are the values to be used for the actual configuration
	float getMouseSensitivity();
	int getSoundVolume();

protected:
	std::map<std::string, StatusItem> m_statusValueMap;
	UserSettings m_userSettings{};


};

