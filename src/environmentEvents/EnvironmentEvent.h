#pragma once
#include <string>
#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <assert.h>
#include <memory>

#include "../Timer.h"

class GameObject;

enum class EventStatus {

	IDLE,
	IN_PROGRESS,
	COMPLETE

};

class EnvironmentEvent
{
public:

	EnvironmentEvent() = default;
	EnvironmentEvent(Timer durationTimer) : m_durationTimer(durationTimer) {};

	std::string label() { return m_label; }
	void setLabel(std::string label) { m_label = label; }
	bool hasExpired() { return m_durationTimer.hasMetTargetDuration(); }
	EventStatus status() { return m_status; }

	virtual std::vector<std::shared_ptr<EnvironmentEvent>> perform(GameObject* envioronmentObject) = 0;

	virtual void stop(GameObject* envioronmentObject) = 0;

protected:
	std::string m_label{};
	int mType{};
	EventStatus m_status{EventStatus::IDLE};
	
	Timer m_durationTimer{};



};

