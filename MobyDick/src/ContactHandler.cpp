#include "ContactHandler.h"
#include "SceneManager.h"




void ContactHandler::handleContacts(const b2WorldId physicsWorldId)
{

	

}

//This function spinds though all sensorEvents from box2d and update the touchingObjects 
// for each gameObject impacted
void ContactHandler::handleSensors(const b2WorldId physicsWorldId)
{


	b2SensorEvents sensorEvents = b2World_GetSensorEvents(physicsWorldId);

	for (int i = 0; i < sensorEvents.beginCount; ++i) {

		const b2SensorBeginTouchEvent& event = sensorEvents.beginEvents[i];

		//Get BodyId and Body User Data
		//This represents the gameObject Itself usually
		b2BodyId sensorBodyId = b2Shape_GetBody(event.sensorShapeId);
		b2BodyId visitorBodyId = b2Shape_GetBody(event.visitorShapeId);
		void* sensorBodyUserData = b2Body_GetUserData(sensorBodyId);
		void* visitorBodyUserData = b2Body_GetUserData(visitorBodyId);

		//Get the GameObjects associated with these bodies
		GameObject* sensorGameObject = reinterpret_cast<GameObject*>(sensorBodyUserData);
		GameObject* visitorGameObject = reinterpret_cast<GameObject*>(visitorBodyUserData);

		if (sensorGameObject->hasTrait(TraitTag::player) ||
			visitorGameObject->hasTrait(TraitTag::player)) {

				int todd = 1;

		}


		//Store this visitor as a touching gameObject to this sensor gameObject
		//First get the main sharedpoint that represents this visitor gamemObject
		auto visitorGameObjectSharedPtr = sensorGameObject->parentScene()->getGameObject(visitorGameObject->id());
		if (visitorGameObjectSharedPtr) {

			sensorGameObject->addTouchingObject(visitorGameObjectSharedPtr.value());

		}

		//Store this Sensor game obejct as a touching gameObject to this visitor gameObject
		//First get the main sharedpoint that represents this sensor gamemObject
		auto sensorGameObjectSharedPtr = sensorGameObject->parentScene()->getGameObject(sensorGameObject->id());
		if (visitorGameObjectSharedPtr) {

			visitorGameObject->addTouchingObject(sensorGameObjectSharedPtr.value());

		}

	}

	for (int i = 0; i < sensorEvents.endCount; ++i) {

		const b2SensorEndTouchEvent& event = sensorEvents.endEvents[i];

		//Get BodyId and Body User Data
		//This represents the gameObject Itself usually
		b2BodyId sensorBodyId = b2Shape_GetBody(event.sensorShapeId);
		b2BodyId visitorBodyId = b2Shape_GetBody(event.visitorShapeId);
		void* sensorBodyUserData = b2Body_GetUserData(sensorBodyId);
		void* visitorBodyUserData = b2Body_GetUserData(visitorBodyId);

		//Get the GameObjects associated with these bodies
		GameObject* sensorGameObject = reinterpret_cast<GameObject*>(sensorBodyUserData);
		GameObject* visitorGameObject = reinterpret_cast<GameObject*>(visitorBodyUserData);

		//Remove this visitor gameObject from the sensor gameObjects touching list
		sensorGameObject->removeTouchingObject(visitorGameObject);

		//Remove this sensor gameObject from the visitor gameObjects touching list
		visitorGameObject->removeTouchingObject(sensorGameObject);

	}

}







