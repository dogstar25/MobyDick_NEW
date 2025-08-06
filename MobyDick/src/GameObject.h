#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <iostream>
#include <json/json.h>
#include <map>
#include <unordered_map>
#include <typeindex>
#include <array>
#include <bitset>
#include <cstdint>

#include <box2d/box2d.h>

#include "BaseConstants.h"

#include "GameObjectDefinition.h"
#include "components/TransformComponent.h"
#include "components/ActionComponent.h"
#include "components/AnimationComponent.h"
#include "components/AttachmentsComponent.h"
#include "components/ChildrenComponent.h"
#include "components/CompositeComponent.h"
#include "components/NavigationComponent.h"
#include "components/ParticleXComponent.h"
#include "components/HudComponent.h"
#include "components/InventoryComponent.h"
#include "components/IMGuiComponent.h"
#include "components/ParticleComponent.h"
#include "components/PhysicsComponent.h"
#include "components/PlayerControlComponent.h"
#include "components/RenderComponent.h"
#include "components/VitalityComponent.h"
#include "components/WeaponComponent.h"
#include "components/PoolComponent.h"


class Scene;
struct SeenObjectDetails;

class GameObject
{
public:

	GameObject() {}
	~GameObject();
	
	//Need to define default move constructors because we have an explicit deconstructor defined
	GameObject(GameObject&&) = default;
	GameObject& operator=(GameObject&&) = default;

	GameObject(std::string gameObjectType, GameObject* parent, float xMapPos, float yMapPos, float angleAdjust, Scene* parentScene, 
		GameLayer layer=GameLayer::MAIN, bool cameraFollow = false, std::string name = "", b2Vec2 sizeOverride = { 0.,0. });

	virtual void update();
	virtual void render();
	virtual void render(SDL_FRect destQuad);
	void render(SDL_FPoint locationPoint);

	void setRemoveFromWorld(bool removeFromWorld) { m_removeFromWorld = removeFromWorld; }
	void setPosition(b2Vec2 position, float angle);
	void setPosition(float x, float y);
	void setPosition(SDL_FPoint position);
	void setPosition(PositionAlignment windowPosition, float adjustX=0., float adjustY=0.);
	void setLayer(GameLayer layer) { m_layer = layer; }
	void setParentScene(Scene* parentScene);
	void setAngleInDegrees(float angle);
	void setAngleInRadians(float angle);
	void setAbsolutePositionaing(bool absolutePositionaing);
	void revertToOriginalAbsolutePositionaing();
	void setColor(SDL_Color color);
	void setTexture(std::string textureId);
	void setWeaponForce(float force);
	void setWeaponColor(SDL_Color color);
	void setCompositePieceLevelCap(int levelCap);
	void setOperatingSound(std::string soundAssetId);
	void setBrainSensorSize(int brainSensorSize);
	void setContainerResapwnTimer(float containerResapwnTimer);
	void setContainerStartCount(int containerStartCount);
	void setContainerCapacity(int containerCapacity);
	void setSize(b2Vec2 size);
	void setSizeToWorldViewOverride();
	bool hasSizeToWorldViewOverride();
	void setDescription(std::string description);
	bool intersectsWith(GameObject* gameObject);
	void setWindowRelativePosition(PositionAlignment windowPosition, float adjustX, float adjustY);
	void addLitHighlight(b2Vec2 size);
	void setLightBrightness(int brightness);

	std::optional<int> renderOrder() { return m_renderOrder; }
	void setRenderOrder(int renderOrder) { m_renderOrder = renderOrder; }

	b2Vec2 getSize();
	SDL_Color getColor();
	bool hasIgnoreTouchTrait(GameObject* gameObject);
	float getAngle();
	float getAngleInDegrees();
	SDL_FPoint getCenterPosition();
	SDL_FPoint getTopLeftPosition();
	SDL_FRect getPositionRect();
	SDL_FPoint getOriginalPosition();
	SDL_FPoint getOriginalTilePosition();
	std::vector<SeenObjectDetails> getSeenObjects();

	bool isPointingAt(SDL_FPoint gameObject);
	bool holdsDependentGameObjects();

	void postInit();
	
	bool hasTrait(int32_t trait) { return m_traitTags.test(trait); }
	std::bitset<100> traits() {	return m_traitTags;	}
	void addTrait(int32_t trait) { m_traitTags.set(trait, true); }
	

	bool hasState(GameObjectState state);
	void addState(GameObjectState state);
	void removeState(GameObjectState state);

	void dispatch(SDL_FPoint destination);
	int brainState();
	bool isAlive();
	bool isCompositeEmpty();
	void stash();
	bool isOffGrid();

	void disableUpdate();
	void enableUpdate();
	bool updateDisabled();

	void disablePhysics();
	void enablePhysics();
	bool physicsDisabled();

	void disableRender();
	void enableRender();
	bool renderDisabled();

	void disableCollision();
	void enableCollision();
	bool collisionDisabled();

	//Accessor Functions
	const std::string& id() const { return m_id; }
	const std::string& name() const { return m_name; }
	const std::string& type() const { return m_type; }
	const std::string& description() const {return m_description; }
	bool absolutePositioning();

	bool isChild{};

	auto removeFromWorld() { return m_removeFromWorld; }
	
	auto const& gameObjectDefinition() { return m_gameObjectDefinition; }
	auto& components() { return m_components; }
	Scene* parentScene() { return m_parentScene; }
	void setCollisionTag(int contactTag);
	GameLayer layer() { return m_layer; }

	//void setCollisionTag(int contactTag) { m_contactTag = contactTag; }
	//void resetCollisionTag() { m_contactTag = m_originalCollisionTag; }

	void reset();
	void addInventoryItem(GameObject* gameObject);
	std::vector<std::weak_ptr<GameObject>> getTouchingByTrait(const int trait);
	std::optional<std::weak_ptr<GameObject>> getFirstTouchingByTrait(const int trait);
	std::optional<std::weak_ptr<GameObject>> getFirstTouchingByType(std::string type);
	bool isTouchingByTrait(const int trait);
	bool isTouchingByType(const std::string type);
	bool isTouchingByName(std::string name);
	bool isTouchingById(const std::string id);
	void addTouchingObject(std::shared_ptr<GameObject> touchingObject);
	void removeTouchingObject(const GameObject* touchingObject);
	void setParent(GameObject* parentObject);
	std::optional<GameObject*> parent() { return m_parentObject; }
	bool isDragging();
	void clearDragging();
	void revertToOriginalSize();

	const std::unordered_map<std::string, std::weak_ptr<GameObject>>& getTouchingObjects() {
		return m_touchingGameObjects;
	}

	inline std::shared_ptr<Component> addComponent(std::shared_ptr<Component> component)
	{

		assert(component->componentType() != ComponentTypes::NONE && "Can't have a component without a ComponentType!");

		auto componentType = component->componentType();
		m_components[(int)componentType] = std::move(component);
		return m_components.at((int)componentType);
	}


	template <typename componentType>
	inline std::shared_ptr<componentType> getComponent(const int componentTypeIndex)
	{

		if (hasComponent(componentTypeIndex))
		{
			return std::static_pointer_cast<componentType>(m_components.at((int)componentTypeIndex));
		}
		else
		{
			return nullptr;
		}
	}

	bool hasComponent(const int componentTypeIndex) {

		if (!m_components[(int)componentTypeIndex]) {
			return false;
		}
		else {
			return true;
		}
	}

	bool operator==(GameObject &gameObject2) {

		if (this->m_id == gameObject2.m_id) {

			return true;

		}
		return false;

	}

	

private:

	std::string m_id{};
	std::string m_name{};
	std::string m_type{};
	std::string m_description{};
	int m_contactTag{ 0 };
	Timer m_touchUpdateTimer{ 0.25 , true};

	std::optional<GameObject*> m_parentObject{};
	
	bool m_isTouchCaptureRequired{};
	bool m_removeFromWorld{ false };
	Scene* m_parentScene{nullptr};
	std::bitset<100> m_traitTags{};
	std::unordered_map<std::string, std::weak_ptr<GameObject>> m_touchingGameObjects{};

	GameObjectDefinition m_gameObjectDefinition;

	GameLayer m_layer;
	std::optional<int> m_renderOrder{};

	//Components
	std::array<std::shared_ptr<Component>, static_cast<int>(ComponentTypes::MAX_COMPONENT_TYPES)>m_components;

	std::string _buildId(GameLayer layer, std::string gameObjectType, float xMapPos, float yMapPos);
	std::string _buildName(std::string rootName, std::string gameObjectType, bool isDependent);
	
	void _imGuiDebugObject();

	//moving this to private. Traits should not be removed. If so, it should be a state not a trait
	void removeTrait(int32_t trait) { m_traitTags.set(trait, false); }

};



