#ifndef CONTACT_LISTENER_H
#define CONTACT_LISTENER_H

#include <string>
#include <vector>
#include <array>

#include <box2d/box2d.h>

class GameObject;


class ContactListener
{
public:
	ContactListener() = default;

	void virtual handleContacts(const b2WorldId physicsWorldId);

private:
	
};

#endif

