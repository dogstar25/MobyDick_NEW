#pragma once

#include <string>
#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <assert.h>
#include <json/json.h>
#include "../JsonSerialization.h"
#include "../BaseConstants.h"

class GameObject;

enum class ProgressionStatus {

	IDLE,
	IN_PROGRESS,
	COMPLETE

};

class Action
{
public:

	Action(Json::Value properties, GameObject* parent);
	~Action();

	
	std::string label() const { return m_label; }
	void setLabel(const std::string label) { m_label = label; }
	GameObject* parent() { return m_parent; }
	void setParent(GameObject* gameObject) { m_parent = gameObject; }

	ProgressionStatus status() { return m_status; }

	virtual void update() {};

	//We never want to end up calling the base level action perform. The derived action classes should have setup
	//the proper perform override and whoever is calling these actions should be passing in the correct paramters
	//If we end up calling base, then throw an assert
	//virtual void perform() { assert(false && "An Empty Action was executed!"); };
	virtual void perform(GameObject*) { assert(false && "An Empty Action was executed!"); };
	virtual void perform(GameObject*, int) { assert(false && "An Empty Action was executed!"); };
	virtual void perform(GameObject*, int, int) { assert(false && "An Empty Action was executed!"); };
	virtual void perform(GameObject*, b2Vec2) { assert(false && "An Empty Action was executed!"); };
	virtual void perform(GameObject*, b2Vec2, float) { assert(false && "An Empty Action was executed!"); };
	virtual void perform(GameObject*, float) { assert(false && "An Empty Action was executed!"); };
	virtual void perform(SDL_FRect*, glm::vec2, float) { assert(false && "An Empty Action was executed!"); };
	virtual void perform(GameObject*, SDL_Scancode) { assert(false && "An Empty Action was executed!"); };
	virtual void perform(GameObject*, GameObject*, SDL_Scancode) { assert(false && "An Empty Action was executed!"); };
	virtual void perform(GameObject*, GameObject*, b2Vec2) { assert(false && "An Empty Action was executed!"); };
	virtual void perform(GameObject*, GameObject*) { assert(false && "An Empty Action was executed!"); };

protected:
	
	GameObject* m_parent{};
	std::string m_label{};
	Json::Value m_properties{};
	ActionType m_actionType{ActionType::DIRECT};
	ProgressionStatus m_status{ ProgressionStatus::IDLE};


};
