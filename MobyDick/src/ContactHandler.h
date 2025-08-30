#pragma once

#include <string>
#include <vector>
#include <array>

#include <box2d/box2d.h>

class GameObject;


class ContactHandler
{
public:
	ContactHandler() = default;

	void virtual handleContacts(const b2WorldId physicsWorldId);

	void virtual handleSensors(const b2WorldId physicsWorldId);

private:
	
};


