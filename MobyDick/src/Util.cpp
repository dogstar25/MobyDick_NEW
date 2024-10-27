#include "Util.h"

#include "EnumMap.h"
#include <random>
#include <format>
#include <algorithm>
#include <math.h>
#include "GameConfig.h"
#include <iostream>
#include "Scene.h"
#include "Game.h"
#include "RayCastCallBack.h"

#define NOMINMAX 10000000 // need this so that the compiler doesnt find the min and max in the windows include
#include <Windows.h>

extern std::unique_ptr<Game> game;


namespace util
{


	std::string wideStringToString(const std::wstring& wstr)
	{
		if (wstr.empty()) return std::string();
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	bool isMouseOverGameObject(SDL_FRect gameObjectRenderDest)
	{

		int mouseX;
		int mouseY;

		const uint32_t currentMouseStates = SDL_GetMouseState(&mouseX, &mouseY);
		SDL_FPoint mouseLocation = { (float)mouseX , (float)mouseY };

		return  SDL_PointInFRect(&mouseLocation, &gameObjectRenderDest);

	}

	SDL_FPoint getMouseWorldPosition()
	{

		int mouseX;
		int mouseY;

		const uint32_t currentMouseStates = SDL_GetMouseState(&mouseX, &mouseY);
		SDL_FPoint mouseLocation = { (float)mouseX , (float)mouseY };

		SDL_FPoint worldPosition = screenToWorldPosition(mouseLocation);

		return worldPosition;

	}

	SDL_FPoint screenToWorldPosition(SDL_FPoint screenPosition)
	{
		SDL_FPoint worldPosition{};
		worldPosition.x = screenPosition.x += Camera::instance().frame().x;
		worldPosition.y = screenPosition.y += Camera::instance().frame().y;

		return  worldPosition;

	}

	void sendSceneEvent(const int sceneActionCode, const std::string& sceneActionCodeId)
	{

		SDL_Event event;

		auto* sceneAction = new std::optional<SceneAction>();
		sceneAction->emplace();  // Initialize the optional with a SceneAction object
		sceneAction->value().actionCode = sceneActionCode;
		sceneAction->value().actionId = sceneActionCodeId;

		event.type = SDL_USEREVENT;
		event.user.data1 = sceneAction;  // Store the pointer in data1
		SDL_PushEvent(&event);


	}

	const int generateRandomNumber(int min, int max)
	{

		if (min == max) {
			return min;
		}


		/*srand((unsigned)time(0));
		int randomNumber;
		randomNumber = (rand() % max) + min;*/


		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<double> dist(min, max);

		return (int)dist(mt);
	}

	const float generateRandomNumber(float min, float max)
	{
		if (min == max) {
			return min;
		}

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<double> dist(min, max);

		return (float)dist(mt);
	}

	const SDL_Color generateRandomColor()
	{
		Uint8 red, green, blue;

		red = generateRandomNumber(0, 255);
		green = generateRandomNumber(0, 255);
		blue = generateRandomNumber(0, 255);
		SDL_Color color;
		color.a = 255;
		color.r = red;
		color.g = green;
		color.b = blue;

		return color;

	}

	const SDL_Color generateRandomColor(SDL_Color beginRange, SDL_Color endRange)
	{
		Uint8 red, green, blue, alpha;

		red = generateRandomNumber(beginRange.r, endRange.r);
		green = generateRandomNumber(beginRange.g, endRange.g);
		blue = generateRandomNumber(beginRange.b, endRange.b);
		alpha = generateRandomNumber(beginRange.a, endRange.a);
		SDL_Color color = {red, green, blue, alpha};

		return color;

	}

	const float radiansToDegrees(float angleInRadians)
	{
		float angle = angleInRadians / b2_pi * 180;
		return angle;

	}

	const float degreesToRadians(float angleInDegrees)
	{
		float angle = angleInDegrees * 0.0174532925199432957f;
		return angle;

	}

	const std::string floatToString(float x, int decDigits)
	{
		//std::stringstream ss;
		//ss << std::fixed;
		//ss.precision(decDigits); // set # places after decimal
		//ss << x;
		//return ss.str();

		auto formattedValue = std::format("{:.{}f}", x, decDigits);
		return formattedValue;

	}

	const SDL_Color JsonToColor(Json::Value JsonColor) 
	{

		SDL_Color color;
		color.r = JsonColor["red"].asInt();
		color.b= JsonColor["blue"].asInt();
		color.g = JsonColor["green"].asInt();
		color.a = JsonColor["alpha"].asInt();

		return color;

	}

	void colorApplyAlpha(SDL_Color& color, int alpha)
	{
		color.a = alpha;
	}

	void colorApplyAlpha(ImVec4& color, int alpha)
	{
		color.w = ((float)alpha/255);
	}

	// To be used when placing an object on a specific tile
	//This takes the objects size into consideration so that , for example
	//A 128 pixel object doesnt get placed with its body crossing into other possible wall tiles
	SDL_FPoint tileToPixelPlacementLocation(float tileX, float tileY, float objectWidth, float objectHeight)
	{
		SDL_FPoint position{};

		position = { 
			(tileX * game->worldTileSize().x) + objectWidth / 2, 
			(tileY * game->worldTileSize().y) + objectHeight / 2	
		};

		return position;
	}

	//To be used for STRICTLY calculating the center of a given tile location
	SDL_FPoint tileToPixelLocation(float tileX, float tileY)
	{
		SDL_FPoint position{};

		position = {
			(tileX * game->worldTileSize().x) + (game->worldTileSize().x / 2),
			(tileY * game->worldTileSize().y) + (game->worldTileSize().y / 2)
		};

		return position;
	}

	SDL_Point pixelToTileLocation(float pixelX, float pixelY)
	{
		SDL_Point position{};

		position = {
			std::max(0, (int)trunc (pixelX / game->worldTileSize().x)),
			std::max(0, (int)trunc (pixelY / game->worldTileSize().y))
		};

		return position;
	}

	std::string locationToString(float x, float y)
	{
		char buffer[10];
		
		snprintf(buffer, 10, "%04d_%04d", (int)x, (int)y);
		auto position = std::string(buffer, 10);

		//auto position = std::format("{:04}_{:04}", (int)x, (int)y);

		return position;
	}

	Json::Value getModelComponent(std::string componentId, std::string modelId)
	{
		std::string definitionFilename = "assets/gameObjectDefinitions/models/" + componentId + "_models.json";

		Json::Value root;
		std::ifstream definitionFile(definitionFilename);
		definitionFile >> root;

		if (root.isMember(modelId)) {

			if (root[modelId]["id"].asString() == componentId) {

				return root[modelId];
			}
			else {
				SDL_assert(false && "Component model is wrong type!");
			}
		}
		else {
			SDL_assert(false && "Component model name not found!");
		}

		return Json::Value();
	}

	const ImVec4 JsonToImVec4Color(Json::Value JsonColor) 
	{

		SDL_Color sdlColor = JsonToColor(JsonColor);

		ImVec4 color = { (float)sdlColor.r / 255, (float)sdlColor.g / 255, (float)sdlColor.b / 255, (float)sdlColor.a / 255 };

		return color;

	}

	const ImVec4 SDLColorToImVec4(SDL_Color sdlColor) 
	{

		ImVec4 color = { (float)sdlColor.r / 255, (float)sdlColor.g / 255, (float)sdlColor.b / 255, (float)sdlColor.a / 255 };

		return color;

	}

	b2Vec2 matchParentRotation(SDL_FPoint childPosition, SDL_FPoint parentPosition, float parentAngle)
	{
		b2Vec2 adjustment;

		//calculate radius of circle defined by parent and initial child position
		//This is the hypotenuse
		float radius = 0;
		radius = sqrt(powf((childPosition.x - parentPosition.x), 2) + powf((childPosition.y - parentPosition.y), 2));

		//calculate the angle of where child is at
		float y = childPosition.y - parentPosition.y;
		float x = childPosition.x - parentPosition.x;
		float childAngle = atan2(childPosition.y - parentPosition.y, childPosition.x - parentPosition.x);

		//add parent angle
		float newAngle = childAngle + util::degreesToRadians(parentAngle);
		b2Vec2 newCenterPosition{};
		newCenterPosition.x = (radius * cos(newAngle));
		newCenterPosition.y = (radius * sin(newAngle));

		newCenterPosition.x += parentPosition.x;
		newCenterPosition.y += parentPosition.y;

		adjustment.x = newCenterPosition.x - childPosition.x;
		adjustment.y = newCenterPosition.y - childPosition.y;

		return adjustment;
	}

	float normalizeRadians(float angleInRadians)
	{

		while (angleInRadians < degreesToRadians(0)) {
			angleInRadians += degreesToRadians(360);
		}

		while (angleInRadians > degreesToRadians(360)) {
			angleInRadians -= degreesToRadians(360);
		}

		return angleInRadians;

	}

	float normalizeDegrees(float angleInDegrees)
	{

		while (angleInDegrees < -180) {
			angleInDegrees += 360;
		}

		while (angleInDegrees > 180) {
			angleInDegrees -= 360;
		}

		return angleInDegrees;
	}

	glm::vec2 glNormalizeTextureCoords(glm::vec2 textureCoords, glm::vec2 textureSize)
	{
		//glm::vec2 textureCoordNormalized = { (textureCoords.x + .5) / textureSize.x, (textureCoords.y +.5) / textureSize.y };
		glm::vec2 textureCoordNormalized = { (textureCoords.x ) / textureSize.x, (textureCoords.y ) / textureSize.y };
		//glm::vec2 adjustment = { (float)1 / (2 * textureSize.x), (float)1 / (2 * textureSize.y) };

		return textureCoordNormalized;
	}

	glm::vec4 glNormalizeColor(const glm::uvec4 &color)
	{
		glm::vec4 colorNormalized = { (float)color.r / 255, (float)color.g / 255, (float)color.b / 255, (float)color.a / 255 };
		return colorNormalized;
	}

	glm::vec4 glNormalizeColor(const SDL_Color& color)
	{
		glm::vec4 colorNormalized = { (float)color.r, (float)color.g, (float)color.b, (float)color.a };
		return glNormalizeColor(colorNormalized);
	}

	b2Vec2& toBox2dPoint(b2Vec2& point)
	{
		point.x /= (float)GameConfig::instance().scaleFactor();
		point.y /= (float)GameConfig::instance().scaleFactor();

		return point;
	}

	SDL_FPoint& toBox2dPoint(SDL_FPoint& point)
	{
		point.x /= (float)GameConfig::instance().scaleFactor();
		point.y /= (float)GameConfig::instance().scaleFactor();

		return point;
	}

	float& toBox2dPoint(float& value)
	{
		value /= (float)GameConfig::instance().scaleFactor();

		return value;
	}



	std::string getComponentType(Json::Value configJSON)
	{

		std::string id = configJSON["id"].asString();
		std::string extractedId{};

		// Find the position of '[' in the string if it exists then we are dealing with a template
		size_t bracketPos = id.find('[');

		if (id.contains('[')) {

			auto underScorePos = id.find_first_of('_');
			extractedId = id.substr(underScorePos + 1, id.size() - underScorePos - 2);
		}
		else {

			extractedId = id;
		}

		return extractedId;
	}

	Json::Value getComponentConfig(Json::Value definitionJSON, int componentType)
	{
		for (Json::Value componentJSON : definitionJSON["components"]) {

			std::string origComponentId = componentJSON["id"].asString();
			std::string  componentId = getComponentType(componentJSON);

			int type = game->enumMap()->toEnum(componentId);

			if (type == componentType) {

				//If componentJSON has a "[" then get the value such as [item_TRANSFORM_COMPONENT]
				//and retrieve it from the models/directory and return it instead of whats in componentJSON
				if (origComponentId.contains('[')) {

					componentJSON = getModelComponent(componentId, origComponentId);

				}

				return componentJSON;

			}

		}

		std::cout << "ComponentType " << componentType << "not found" << std::endl;
		assert(true && "Component not found");
		return Json::Value();
	}

	b2JointDef* createJoint(b2JointType jointType)
	{
		b2JointDef* jointDef = nullptr;

		if (jointType == b2JointType::e_weldJoint) {

			jointDef = new b2WeldJointDef();
		}
		else if (jointType == b2JointType::e_motorJoint) {

			jointDef = new b2MotorJointDef();
		}

		return jointDef;
		
	}

	float calculateDistance(SDL_FPoint location1, SDL_FPoint location2)
	{

		auto distance = std::powf((location1.x - location2.x), 2) + std::powf((location1.y - location2.y), 2);
		distance = std::sqrtf(distance);

		return distance;
	}

	float calculateDistance(SDL_Point location1, SDL_Point location2)
	{

		auto distance = std::pow((location1.x - location2.x), 2) + std::pow((location1.y - location2.y), 2);
		distance = std::sqrtf(distance);

		return distance;
	}

	float calculateDistance(glm::vec2 location1, glm::vec2 location2)
	{

		auto distance = glm::distance(location1, location2);

		return distance;
	}

	bool fileExists(const std::string& filename)
	{
		struct stat buf;
		if (stat(filename.c_str(), &buf) != -1)
		{
			return true;
		}
		return false;
	}

	const std::string genRandomId(const int len) 
	{
		static char alphanum[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		std::string tmp_s;
		tmp_s.reserve(len);

		for (int i = 0; i < len; ++i) {
			tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
		}

		return tmp_s;
	}


	bool hasLineOfSight(GameObject* sourceObject, GameObject* candidateObject)
	{
		bool clearPath{ true };

		//Special override for things like the doorknob, which is built into the door which itself
		//can become impassable or a barrier
		if (candidateObject->hasTrait(TraitTag::always_in_line_of_sight)) {

			return true;
		}

		b2Vec2 sourcePosition = { sourceObject->getCenterPosition().x, sourceObject->getCenterPosition().y };
		b2Vec2 candidatePosition = { candidateObject->getCenterPosition().x, candidateObject->getCenterPosition().y };

		//convert to box2d coordinates
		util::toBox2dPoint(sourcePosition);
		util::toBox2dPoint(candidatePosition);

		//cast a physics raycast from the light object to the center of this lightedArea's center
		sourceObject->parentScene()->physicsWorld()->RayCast(&RayCastCallBack::instance(), sourcePosition, candidatePosition);

		//Loop through all objects hit between the player and the center of the mask area
		for (BrainRayCastFoundItem rayHitObject : RayCastCallBack::instance().intersectionItems()) {

			//Is this a barrier or and also NOT its own body and the object is not physicsdisabled
			if ((rayHitObject.gameObject->hasTrait(TraitTag::barrier) || rayHitObject.gameObject->hasState(GameObjectState::IMPASSABLE)) &&
				rayHitObject.gameObject != sourceObject) {
				clearPath = false;
				break;
			}
		}

		RayCastCallBack::instance().reset();

		return clearPath;

	}

	//void propogateStateToAllChildren(GameObject* gameObject, GameObjectState stateToPropogate, std::optional<int> trait)
	//{


	//	if (gameObject->hasComponent(ComponentTypes::CHILDREN_COMPONENT)) {

	//		const auto& childComponent = gameObject->getComponent<ChildrenComponent>(ComponentTypes::CHILDREN_COMPONENT);


	//		for (auto& slotItr : childComponent->childSlots()) {

	//			//Each child slot can have multiple gameObjects that live in a vector
	//			//Only Standard slots support multipl
	//			for (auto& child : slotItr.second) {

	//				if (child.gameObject.has_value()) {

	//					if (trait.has_value()==false || (trait.has_value() && child.gameObject.value()->hasTrait(trait.value()))) {

	//						child.gameObject.value()->addState(stateToPropogate);
	//					}

	//				}

	//				propogateStateToAllChildren(child.gameObject.value().get(), stateToPropogate, trait);

	//			}

	//		}

	//	}



	//}

	void replaceAll(std::string& str, const std::string& from, const std::string& to) {
		if (from.empty()) {
			return;
		}
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
	}



}











