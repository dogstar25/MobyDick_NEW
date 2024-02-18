#include "GameObject.h"



#include "GameObjectManager.h"
#include "game.h"
#include "EnumMap.h"

#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "Util.h"
#include "components/LightedTreatmentComponent.h"
#include "components/MaskedOverlayComponent.h"
#include "components/InventoryComponent.h"


extern std::unique_ptr<Game> game;

GameObject::~GameObject()
{

	//m_shouldThreadStop = true;
	//std::cout << this->m_id << " GameObject Destructor called" << std::endl;

}

GameObject::GameObject(std::string gameObjectType, float xMapPos, float yMapPos, float angleAdjust, Scene* parentScene, GameLayer layer, 
	bool cameraFollow, std::string name, b2Vec2 sizeOverride)
{

	Json::Value definitionJSON;

	//Build components
	definitionJSON = GameObjectManager::instance().getDefinition(gameObjectType)->definitionJSON();
	m_gameObjectDefinition = definitionJSON;

	//Category Id and Object Type
	m_type = gameObjectType;
	m_removeFromWorld = false;

	//Description
	m_description = definitionJSON["description"].asString();

	//Layer
	m_layer = layer;

	//Build the unique id
	m_id = _buildId(layer, gameObjectType, xMapPos, yMapPos);

	//Build the unique name if a name wasnt given
	if (name.empty()) {
		m_name = gameObjectType + "_ANON";
	}
	else {
		m_name = name;
	}

	//Trait tags
	for (Json::Value itrControls : definitionJSON["traitTags"])
	{
		uint32_t trait = game->enumMap()->toEnum(itrControls.asString());
		m_traitTags.set(trait);
	}

	//Set the parent Scene
	m_parentScene = parentScene;
	parentScene->incrementGameObjectCount();

	std::shared_ptr<Component> component{};

	for (Json::Value componentJSON : definitionJSON["components"]){

		std::string componentTypeString = util::getComponentType(componentJSON);
		int componentType = game->enumMap()->toEnum(componentTypeString);

		component = game->componentFactory()->create(
			definitionJSON, m_name, gameObjectType, parentScene, xMapPos, yMapPos, angleAdjust, sizeOverride, componentType);
		component->setParent(this);
		addComponent(component);

	}

	//Set the camera to follow this GameObject
	if (cameraFollow) {
		Camera::instance().setFollowMe(this->name());
	}

}

void GameObject::addTouchingObject(std::shared_ptr<GameObject> touchingObject) 
{

	m_touchingGameObjects[touchingObject->id()] = touchingObject;

}

void GameObject::setParent(GameObject* parentObject)
{

	m_parentObject = parentObject;

}

bool GameObject::isDragging()
{
	if (hasComponent(ComponentTypes::INTERFACE_COMPONENT)) {

		return getComponent<InterfaceComponent>(ComponentTypes::INTERFACE_COMPONENT)->isDragging();

	}

	return false;

}

void GameObject::clearDragging()
{
	if (hasComponent(ComponentTypes::INTERFACE_COMPONENT)) {

		getComponent<InterfaceComponent>(ComponentTypes::INTERFACE_COMPONENT)->clearDragging();

	}
}

void GameObject::revertToOriginalSize()
{
	getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT)->revertToOriginalSize();
}

void GameObject::setPosition(float x, float y)
{

	if (hasComponent(ComponentTypes::PHYSICS_COMPONENT)) {
		b2Vec2 b2Point = {x, y};
		auto b2Position = util::toBox2dPoint(b2Point);
		getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT)->setTransform(b2Position);
	}

	getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT)->setPosition(x, y);

}

void GameObject::setPosition(SDL_FPoint position)
{
	if (hasComponent(ComponentTypes::PHYSICS_COMPONENT)) {
		b2Vec2 b2Point = { position.x, position.y };
		auto b2Position = util::toBox2dPoint(b2Point);
		getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT)->setTransform(b2Position);
	}

	getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT)->setPosition(position);

}

void GameObject::setPosition(b2Vec2 position, float angle)
{
	//-1 means don't apply the angle
	if (angle != -1)
	{

		auto radianAngle = util::degreesToRadians(angle);

		if (hasComponent(ComponentTypes::PHYSICS_COMPONENT)) {
			b2Vec2 b2Point = { position.x, position.y };
			auto b2Position = util::toBox2dPoint(b2Point);
			getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT)->setTransform(b2Position, radianAngle);
		}

		getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT)->setPosition(position, angle);
	}
	else
	{
		if (hasComponent(ComponentTypes::PHYSICS_COMPONENT)) {
			b2Vec2 b2Point = { position.x, position.y };
			auto b2Position = util::toBox2dPoint(b2Point);
			getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT)->setTransform(b2Position);
		}

		getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT)->setPosition(position);
	}

}

void GameObject::setWindowRelativePosition(PositionAlignment windowPosition, float adjustX, float adjustY)
{
	const auto& transforComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	transforComponent->setWindowRelativePosition(windowPosition);
	transforComponent->setWindowPositionAdjustment({ adjustX, adjustY });

}

void GameObject::addLitHighlight(b2Vec2 size)
{

	std::shared_ptr<ChildrenComponent> childrenComponent{};

	auto lightObject = m_parentScene->addGameObject("LIGHT_ITEM_HIGHLIGHT_CIRCLE", GameLayer::FOREGROUND_5 , -1.0F, -1.0F,
		0.F, false, "LitHightlight", size);
	std::shared_ptr<GameObject> lightObjectShrPtr = m_parentScene->getGameObject(lightObject->id()).value();
	lightObjectShrPtr->addState(GameObjectState::ON);


	if (hasComponent(ComponentTypes::CHILDREN_COMPONENT)) {

		childrenComponent = getComponent<ChildrenComponent>(ComponentTypes::CHILDREN_COMPONENT);
		childrenComponent->addStepChild(lightObjectShrPtr, PositionAlignment::CENTER);

	}
	else {
		Json::Value componentsDefinition{};
		Json::Value childrenComponentDefinition{};

		childrenComponentDefinition["id"] = "CHILDREN_COMPONENT";
		childrenComponentDefinition["sameSlotTreatment"] = "ChildSlotTreatment::STACKED";
		componentsDefinition["components"].append(childrenComponentDefinition);


		childrenComponent =
			std::static_pointer_cast<ChildrenComponent>(
				game->componentFactory()->create(componentsDefinition, m_name, "", m_parentScene, 0, 0, 0, b2Vec2_zero,
					ComponentTypes::CHILDREN_COMPONENT)
			);
		childrenComponent->setParent(this);
		addComponent(childrenComponent);

		childrenComponent->addStepChild(lightObjectShrPtr, PositionAlignment::CENTER);

	}


}

void GameObject::setPosition(PositionAlignment windowPosition, float adjustX, float adjustY)
{
	float xMapPos{};
	float yMapPos{};

	auto objectWidth = getSize().x;
	auto objectHeight = getSize().y;

	

	if (windowPosition == PositionAlignment::CENTER) {

		xMapPos = (float)game->gameScreenResolution().x / 2;
		yMapPos = (float)game->gameScreenResolution().y / 2;

	}
	else if (windowPosition == PositionAlignment::TOP_CENTER) {

		xMapPos = (float)game->gameScreenResolution().x / 2;
		yMapPos = (objectHeight / 2);
	}
	else if (windowPosition == PositionAlignment::TOP_LEFT) {

		xMapPos = (objectWidth / 2);
		yMapPos = (objectHeight / 2);
	}
	else if (windowPosition == PositionAlignment::TOP_RIGHT) {

		xMapPos = (float)(game->gameScreenResolution().x - (objectWidth / 2));
		yMapPos = (objectHeight / 2);
	}
	else if (windowPosition == PositionAlignment::CENTER_LEFT) {

		xMapPos = (objectWidth / 2);
		yMapPos = (float)game->gameScreenResolution().y / 2;
	}
	else if (windowPosition == PositionAlignment::CENTER_RIGHT) {

		xMapPos = (float)(game->gameScreenResolution().x - (objectWidth / 2));
		yMapPos = (float)game->gameScreenResolution().y / 2;
	}
	else if (windowPosition == PositionAlignment::BOTTOM_LEFT) {

		xMapPos = (objectWidth / 2);
		yMapPos = (float)(game->gameScreenResolution().y - objectHeight);
	}
	else if (windowPosition == PositionAlignment::BOTTOM_CENTER) {

		xMapPos = (float)(game->gameScreenResolution().x / 2);
		yMapPos = (float)(game->gameScreenResolution().y - objectHeight);
	}
	else if (windowPosition == PositionAlignment::BOTTOM_RIGHT) {

		xMapPos = (float)(game->gameScreenResolution().x - (objectWidth / 2));
		yMapPos = (float)(game->gameScreenResolution().y - objectHeight);
	}

	//Add adjustments
	xMapPos += adjustX;
	yMapPos += adjustY;

	setPosition(xMapPos, yMapPos);


}

void GameObject::update()
{

	if (this->updateDisabled() == false) {
		for (auto& component : m_components)
		{
			if (component && component->isDisabled() == false) {
				component->update();
			}
		}

		_updateTouchingObjects();

	}

}

/*
Render this object somewhere other than the GameObjects transformcomponent location
*/
void GameObject::render(SDL_FRect destQuad)
{
	getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT)->render(destQuad);

}

void GameObject::render(SDL_FPoint locationPoint)
{

	getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT)->render(locationPoint);

}

void GameObject::render()
{

	//Special spot to place a debug ImGui object
	_imGuiDebugObject();

	if (this->renderDisabled() == false) {

		//Render yourself
		getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT)->render();

		//Render your children
		if (hasComponent(ComponentTypes::CHILDREN_COMPONENT)) {

			getComponent<ChildrenComponent>(ComponentTypes::CHILDREN_COMPONENT)->render();
		}

		//Render your attached items
		if (hasComponent(ComponentTypes::ATTACHMENTS_COMPONENT)) {

			getComponent<AttachmentsComponent>(ComponentTypes::ATTACHMENTS_COMPONENT)->render();
		}

		//If you have an arcade particle emitter then render those particles
		if (hasComponent(ComponentTypes::PARTICLE_COMPONENT)) {

			getComponent<ParticleComponent>(ComponentTypes::PARTICLE_COMPONENT)->render();
		}

		//If you have a composite component, then render the composite pieces
		if (hasComponent(ComponentTypes::COMPOSITE_COMPONENT)) {

			getComponent<CompositeComponent>(ComponentTypes::COMPOSITE_COMPONENT)->render();
		}

		//If you have a hud component, then render the hud items
		if (hasComponent(ComponentTypes::HUD_COMPONENT)) {

			getComponent<HudComponent>(ComponentTypes::HUD_COMPONENT)->render();
		}

		//If you have a IMGui component, then render it
		if (hasComponent(ComponentTypes::IMGUI_COMPONENT)) {

			getComponent<IMGuiComponent>(ComponentTypes::IMGUI_COMPONENT)->render();
		}

		//If you have an Interface component, then render possible interaction menus
		if (hasComponent(ComponentTypes::INTERFACE_COMPONENT)) {

			getComponent<InterfaceComponent>(ComponentTypes::INTERFACE_COMPONENT)->render();
		}

		//If you have a Container component, then render the container content items
		if (hasComponent(ComponentTypes::CONTAINER_COMPONENT)) {

			getComponent<ContainerComponent>(ComponentTypes::CONTAINER_COMPONENT)->render();
		}

		////Render Inventory
		//if (hasComponent(ComponentTypes::INVENTORY_COMPONENT)) {

		//	getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT)->render();
		//}

		//Render Inventory
		if (hasComponent(ComponentTypes::GRID_DISPLAY_COMPONENT)) {

			getComponent<GridDisplayComponent>(ComponentTypes::GRID_DISPLAY_COMPONENT)->render();
		}

		//If you have a Light treatment component, then render it
		if (hasComponent(ComponentTypes::LIGHTED_TREATMENT_COMPONENT)) {

			getComponent<LightedTreatmentComponent>(ComponentTypes::LIGHTED_TREATMENT_COMPONENT)->render();
		}

		//If you have a Light treatment component, then render it
		if (hasComponent(ComponentTypes::MASKED_OVERLAY_COMPONENT)) {

			getComponent<MaskedOverlayComponent>(ComponentTypes::MASKED_OVERLAY_COMPONENT)->render();
		}

	}
}

bool GameObject::intersectsWith(GameObject* gameObject)
{

	//Do not store yourself as a touching object
	if (this == gameObject) {
		return false;
	}

	const auto& compareObjectTransformComponent = gameObject->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	const auto& thisObjectTransformComponent = this->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);

	SDL_FRect compareObjectRect{};
	SDL_FRect thisObjectRect{};

	//If either one of the objects are absolute positioned, then use there render rectangle to determin eif touching
	//otherwise use there worl position
	if (compareObjectTransformComponent->absolutePositioning() == true ||
		thisObjectTransformComponent->absolutePositioning() == true) {

		const auto& compareObjectRenderComponent = gameObject->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
		const auto& thisObjectRenderComponent = this->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

		compareObjectRect = compareObjectRenderComponent->getRenderDestRect();
		thisObjectRect = thisObjectRenderComponent->getRenderDestRect();

	}
	else
	{
		compareObjectRect = compareObjectTransformComponent->getPositionRect();
		thisObjectRect = thisObjectTransformComponent->getPositionRect();

	}

	if (SDL_HasIntersectionF(&compareObjectRect, &thisObjectRect) ) {
		return true;
	}
	else {
		return false;
	}
}


void GameObject::reset()
{

	if (hasComponent(ComponentTypes::POOL_COMPONENT)) {
		getComponent<PoolComponent>(ComponentTypes::POOL_COMPONENT)->reset();
	}

	if (hasComponent(ComponentTypes::PHYSICS_COMPONENT)) {
		getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT)->stash();
		getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT)->update();
	}

	//test calling the main update so that if the pooled item has a child, it needs to be moved off too
	update();


}

void GameObject::addInventoryItem( GameObject* gameObject)
{
	//size_t itemCount = getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT)->addItem(gameObject);
	//If this is the only inventory item, then attach it to the player of whatever object this is
	/*if (itemCount == 1)
	{
		getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT)->attachItem(gameObject);
	}*/

}

void GameObject::setPhysicsActive(bool active)
{

	const auto& physicsComponent = getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
	if (physicsComponent) {
		physicsComponent->setPhysicsBodyActive(active);
	}

}

void GameObject::setAngleInRadians(float angle)
{

	const auto& physicsComponent = getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
	if (physicsComponent) {

		physicsComponent->setAngle(angle);
	}

}

void GameObject::setAbsolutePositionaing(bool absolutePositionaing)
{

	const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	if (transformComponent) {

		transformComponent->setAbsolutePositioning(absolutePositionaing);
	}

}

bool GameObject::absolutePositioning()
{

	const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	return transformComponent->absolutePositioning();

}

void GameObject::revertToOriginalAbsolutePositionaing()
{

	const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	if (transformComponent) {

		transformComponent->setAbsolutePositioning(transformComponent->originalAbsolutePositioning());
	}

}

void GameObject::setAngleInDegrees(float angle)
{

	const auto& physicsComponent = getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
	if (physicsComponent) {

		angle = util::degreesToRadians(angle);

		physicsComponent->setAngle(angle);
	}

}

float GameObject::getAngle()
{

	const auto& physicsComponent = getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
	if (physicsComponent) {

		return physicsComponent->angle();
	}
	else {
		const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
		if (transformComponent) {
			return util::degreesToRadians(transformComponent->angle());
		}
	}

	return 0;
}

float GameObject::getAngleInDegrees()
{

	const auto& physicsComponent = getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
	if (physicsComponent) {

		return util::radiansToDegrees(physicsComponent->angle());
	}
	else {
		const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
		if (transformComponent) {
			return transformComponent->angle();
		}
	}


	return 0;
}

bool GameObject::holdsDependentGameObjects()
{
	if (hasComponent(ComponentTypes::CHILDREN_COMPONENT) ||
		hasComponent(ComponentTypes::COMPOSITE_COMPONENT) ||
		hasComponent(ComponentTypes::ATTACHMENTS_COMPONENT) ||
		hasComponent(ComponentTypes::INVENTORY_COMPONENT) ||
		hasComponent(ComponentTypes::CONTAINER_COMPONENT)) 
	{
		return true;
	}
	else {
		return false;
	}

}

/*
The postInit function allows for initialization that requires all objects in the game to be 'already' instantiated 
ex. The brainComponent needs all navigation related gameObjects to be built first
OR
For at least all components of a gameObject to be instantiated
ex. The HudComponent requires that the tranform component be instantiated so that it can set some of its properties
*/
void GameObject::postInit()
{

	for (int i = 0; i < ComponentTypes::MAX_COMPONENT_TYPES; i++) {

		if (components()[i]) {

			components()[i]->postInit();

		}
	}

}


void GameObject::setParentScene(Scene* parentScene)
{
	m_parentScene = parentScene;
}


SDL_FPoint GameObject::getCenterPosition()
{

	const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	return(transformComponent->getCenterPosition());

}

SDL_FRect GameObject::getPositionRect()
{

	const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	return(transformComponent->getPositionRect());

}

bool GameObject::isAlive()
{

	//Drive from isAlive vs isDead - inanimate objects are never alive
	bool alive{};

	if (hasComponent(ComponentTypes::VITALITY_COMPONENT)) {

		const auto& vitalityComponent = getComponent<VitalityComponent>(ComponentTypes::VITALITY_COMPONENT);
		alive = !vitalityComponent->isDead();
	}

	return alive;


}

SDL_FPoint GameObject::getOriginalPosition()
{

	const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	SDL_FPoint position = { transformComponent->originalPosition() .x, transformComponent->originalPosition() .y};
	return(position);

}


SDL_FPoint GameObject::getOriginalTilePosition()
{

 	const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	SDL_FPoint position = { transformComponent->originalTilePosition().x, transformComponent->originalTilePosition().y };
	return(position);

}

b2Vec2 GameObject::getSize()
{

	const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	return(transformComponent->size());

}

SDL_FPoint GameObject::getTopLeftPosition()
{

	const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	return(transformComponent->getTopLeftPosition());

}

std::string GameObject::_buildName(std::string rootName, std::string gameObjectType, bool isDependent)
{
	std::string name{};
	if (isDependent) {
		name = name + rootName + gameObjectType;
		//name = std::format("{}_{:.0f}_{:.0f}_{}", rootName, xMapPos, yMapPos, randomid);
	}
	else {

		name = name + rootName + gameObjectType;

	}

	return name;

}

std::string GameObject::_buildId(GameLayer layer, std::string id, float xMapPos, float yMapPos)
{
	std::string randomid = util::genRandomId(16);
	auto name = std::format("{:02}_{}_{:.0f}_{:.0f}_{}", (int)layer, id, xMapPos, yMapPos, randomid);

	return name;

}

bool GameObject::isPointingAt(SDL_FPoint gameObjectPosition)
{

	const std::shared_ptr<PhysicsComponent> physicsComponent = getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
	//const std::shared_ptr<PhysicsComponent> referenceObjectPhysicsComponent = gameObject->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);

	float hostAngleDegrees = util::radiansToDegrees(physicsComponent->angle());
	float orientationAngle = atan2(getCenterPosition().y - gameObjectPosition.y, getCenterPosition().x - gameObjectPosition.x);
	float orientationAngleDegrees = util::radiansToDegrees(orientationAngle);

	if ((hostAngleDegrees - orientationAngleDegrees) >= 140 &&
		(hostAngleDegrees - orientationAngleDegrees) <= 220) {
		return true;
	}
	else {
		return false;
	}


}

bool GameObject::isCompositeEmpty()
{

	if (hasComponent(ComponentTypes::COMPOSITE_COMPONENT) == true) {

		const auto& compositeComponent = getComponent<CompositeComponent>(ComponentTypes::COMPOSITE_COMPONENT);
		if (compositeComponent) {
			return compositeComponent->isCompositeEmpty();
		}
	}

	return false;
}

bool GameObject::isOffGrid()
{
	const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	if (transformComponent->getCenterPosition().x < 0 && transformComponent->getCenterPosition().y < 0) {
		return true;
	}

	return false;
}

//Take the GameObject out of the game but dont delete it so that it can we put back easily
void GameObject::stash()
{

	b2Vec2 positionVector = b2Vec2(-50, -50);

	if (hasComponent(ComponentTypes::PHYSICS_COMPONENT)) {
		const auto& physicsComponent = getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
		b2Vec2 velocityVector = b2Vec2(0, 0);
		

		physicsComponent->physicsBody()->SetTransform(positionVector, 0);
		physicsComponent->physicsBody()->SetLinearVelocity(velocityVector);
		physicsComponent->physicsBody()->SetEnabled(false);

		const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
		transformComponent->setPosition(positionVector);

	}
	else {
		const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
		transformComponent->setPosition(positionVector);
	}

	//If this gameObject has a interface, then make sure that if the current active interface is this guys, then clear it out
	if (hasComponent(ComponentTypes::INTERFACE_COMPONENT)) {

		const auto& interfaceComponent = getComponent<InterfaceComponent>(ComponentTypes::INTERFACE_COMPONENT);
		interfaceComponent->clearSpecificGameObjectInterface(this);
	}
}

bool GameObject::hasState(GameObjectState state)
{
	if (hasComponent(ComponentTypes::STATE_COMPONENT)) {

		const auto& stateComponent = getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);

		return stateComponent->testState(state);

	}

	return false;
	
}

void GameObject::addState(GameObjectState state)
{

	if (hasComponent(ComponentTypes::STATE_COMPONENT)) {

		const auto& stateComponent = getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);

		stateComponent->addState(state);

	}

}

void GameObject::removeState(GameObjectState state)
{

	if (hasComponent(ComponentTypes::STATE_COMPONENT)) {

		const auto& stateComponent = getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);

		stateComponent->removeState(state);

	}

}

void GameObject::dispatch(SDL_FPoint destination)
{
	const auto& brainComponent = getComponent<BrainComponent>(ComponentTypes::BRAIN_COMPONENT);
	//brainComponent->dispatch(destination);
}

int GameObject::brainState()
{
	const auto& brainComponent = getComponent<BrainComponent>(ComponentTypes::BRAIN_COMPONENT);

	return brainComponent->state();
}


void GameObject::disableUpdate()
{
	if (hasComponent(ComponentTypes::STATE_COMPONENT) == true) {

		const auto& stateComponent = getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);
		stateComponent->addState(GameObjectState::DISABLED_UPDATE);
	}

}
void GameObject::enableUpdate()
{

	if (hasComponent(ComponentTypes::STATE_COMPONENT) == true) {

		const auto& stateComponent = getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);
		stateComponent->removeState(GameObjectState::DISABLED_UPDATE);
	}

}
bool GameObject::updateDisabled()
{

	if (hasComponent(ComponentTypes::STATE_COMPONENT) == true) {

		const auto& stateComponent = getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);
		return stateComponent->testState(GameObjectState::DISABLED_UPDATE);
	}

	return false;

}

void GameObject::disablePhysics()
{

	if (hasComponent(ComponentTypes::STATE_COMPONENT) == true) {

		const auto& stateComponent = getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);
		stateComponent->addState(GameObjectState::DISABLED_PHYSICS);
	}

	if (hasComponent(ComponentTypes::PHYSICS_COMPONENT) == true) {

		const auto& physicsComponent = getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
		if (physicsComponent) {
			physicsComponent->setPhysicsBodyActive(false);
		}
	}

}

void GameObject::enablePhysics()
{
	if (hasComponent(ComponentTypes::STATE_COMPONENT) == true) {

		const auto& stateComponent = getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);
		stateComponent->removeState(GameObjectState::DISABLED_PHYSICS);
	}

	if (hasComponent(ComponentTypes::PHYSICS_COMPONENT) == true) {

		const auto& physicsComponent = getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
		if (physicsComponent) {
			physicsComponent->setPhysicsBodyActive(true);
		}

	}

}

bool GameObject::physicsDisabled()
{

	if (hasComponent(ComponentTypes::STATE_COMPONENT) == true) {

		const auto& stateComponent = getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);
		return stateComponent->testState(GameObjectState::DISABLED_PHYSICS);
	}

	return false;

}

void GameObject::disableRender()
{

	if (hasComponent(ComponentTypes::STATE_COMPONENT) == true) {

		const auto& stateComponent = getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);
		stateComponent->addState(GameObjectState::DISABLED_RENDER);
	}

}

void GameObject::enableRender()
{

	if (hasComponent(ComponentTypes::STATE_COMPONENT) == true) {

		const auto& stateComponent = getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);
		stateComponent->removeState(GameObjectState::DISABLED_RENDER);
	}

}

bool GameObject::renderDisabled()
{
	if (hasComponent(ComponentTypes::STATE_COMPONENT) == true) {

		const auto& stateComponent = getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);
		return stateComponent->testState(GameObjectState::DISABLED_RENDER);
	}

	return false;

}

void GameObject::disableCollision(bool includeSensors)
{

	if (hasComponent(ComponentTypes::STATE_COMPONENT) == true) {

		const auto& stateComponent = getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);
		stateComponent->addState(GameObjectState::DISABLED_COLLISION);
	}

	if (hasComponent(ComponentTypes::PHYSICS_COMPONENT) == true) {
		const auto& physicsComponent = getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
		for (auto fixture = physicsComponent->physicsBody()->GetFixtureList(); fixture != 0; fixture = fixture->GetNext())
		{
			if (fixture->IsSensor() == false || (fixture->IsSensor() == true && includeSensors == true)) {
				ContactDefinition* contactDefinition = reinterpret_cast<ContactDefinition*>(fixture->GetUserData().pointer);
				contactDefinition->contactTag = ContactTag::GENERAL_FREE;
				fixture->Refilter();
			}
		}
	}

}

void GameObject::enableCollision()
{
	if (hasComponent(ComponentTypes::STATE_COMPONENT) == true) {

		const auto& stateComponent = getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);
		stateComponent->removeState(GameObjectState::DISABLED_COLLISION);
	}

	if (hasComponent(ComponentTypes::PHYSICS_COMPONENT) == true) {
		const auto& physicsComponent = getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
		for (auto fixture = physicsComponent->physicsBody()->GetFixtureList(); fixture != 0; fixture = fixture->GetNext())
		{
			ContactDefinition* contactDefinition = reinterpret_cast<ContactDefinition*>(fixture->GetUserData().pointer);
			contactDefinition->contactTag = contactDefinition->saveOriginalContactTag;
			fixture->Refilter();
		}
	}

}

bool GameObject::collisionDisabled()
{

	if (hasComponent(ComponentTypes::STATE_COMPONENT) == true) {

		const auto& stateComponent = getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);
		return stateComponent->testState(GameObjectState::DISABLED_COLLISION);
	}

	return false;

}


void GameObject::setCollisionTag( int contactTag)
{

	if (hasComponent(ComponentTypes::PHYSICS_COMPONENT) == true) {
		const auto& physicsComponent = getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
		for (auto fixture = physicsComponent->physicsBody()->GetFixtureList(); fixture != 0; fixture = fixture->GetNext())
		{
			ContactDefinition* contactDefinition = reinterpret_cast<ContactDefinition*>(fixture->GetUserData().pointer);
			contactDefinition->contactTag = contactTag;
		}
	}

}

void GameObject::setColor(SDL_Color color)
{

	const auto& renderComponent = getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	renderComponent->setColor(color);

}

void GameObject::setSize(b2Vec2 size)
{

	//This should really only be used for non-physics objects because it will not affect the 
	//physics size of the object (cant dynamically change a physics objects size)
	const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	transformComponent->setSize(size);

}

void GameObject::setWeaponColor(SDL_Color color)
{

	const auto& weaponComponent = getComponent<WeaponComponent>(ComponentTypes::WEAPON_COMPONENT);
	weaponComponent->setColor(color);

}

void GameObject::setWeaponForce(float force)
{

	const auto& weaponComponent = getComponent<WeaponComponent>(ComponentTypes::WEAPON_COMPONENT);
	weaponComponent->setForce(force);

}

void GameObject::setCompositePieceLevelCap(int levelCap)
{

	const auto& compositeComponent = getComponent<CompositeComponent>(ComponentTypes::COMPOSITE_COMPONENT);
	for (auto& piece : compositeComponent->pieces()) {

		const auto& vitalityComponent = piece.pieceObject->getComponent<VitalityComponent>(ComponentTypes::VITALITY_COMPONENT);
		vitalityComponent->setLevelCap(levelCap);

	}

}

void GameObject::setBrainSensorSize(int brainSensorSize)
{

	const auto& brainComponent = getComponent<BrainComponent>(ComponentTypes::BRAIN_COMPONENT);
	brainComponent->setSightSensorSize(brainSensorSize);

}

void GameObject::setContainerResapwnTimer(float containerResapwnTimer)
{

	const auto& containerComponent = getComponent<ContainerComponent>(ComponentTypes::CONTAINER_COMPONENT);
	containerComponent->setRefillTimer(containerResapwnTimer);

}

void GameObject::setContainerStartCount(int containerStartCount)
{

	const auto& containerComponent = getComponent<ContainerComponent>(ComponentTypes::CONTAINER_COMPONENT);
	containerComponent->setStartCount(containerStartCount);

}

void GameObject::setContainerCapacity(int containerCapacity)
{

	const auto& containerComponent = getComponent<ContainerComponent>(ComponentTypes::CONTAINER_COMPONENT);
	containerComponent->setCapacity(containerCapacity);

}

void GameObject::setOperatingSound(std::string soundAssetId)
{

	const auto& soundComponent = getComponent<SoundComponent>(ComponentTypes::SOUND_COMPONENT);
	std::optional<SoundItem> sound{};

	sound = soundComponent->getSound("OPERATING_SOUND");
	if (sound.has_value()) {

		sound->soundAssetId = soundAssetId;
	}
	else {

		soundComponent->addSound("OPERATING_SOUND", soundAssetId, false, true);
	}

}

void GameObject::_updateTouchingObjects()
{

		m_touchingGameObjects.clear();

		//If this is a physics GameObject then capture a list of every object that it or its aux sensor is currently touching
		if (this->hasComponent(ComponentTypes::PHYSICS_COMPONENT)) {

			if (this->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT)->isTouchingObjectsCapturedRequired() == true) {

				const std::shared_ptr<PhysicsComponent> physicsComponent = this->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);

				for (b2ContactEdge* edge = physicsComponent->physicsBody()->GetContactList(); edge; edge = edge->next)
				{
					b2Contact* contact = edge->contact;

					//One of these fixtures being reported as a contact is the object itself, so we dont care about that one. 
					// We only care about the objects are are not this object itself
					GameObject* contactGameObject = reinterpret_cast<GameObject*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
					GameObject* contactGameObject2 = reinterpret_cast<GameObject*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

					if (contact->IsTouching()) {

						//const auto manifold = contact->GetManifold();
						//manifold->localNormal;

						if (contactGameObject != this && contactGameObject->hasTrait(TraitTag::fragment) == false) {

							auto contactGameObjectSharedPtr = m_parentScene->getGameObject(contactGameObject->id());
							this->addTouchingObject(contactGameObjectSharedPtr.value());

						}
						else if (contactGameObject2 != this && contactGameObject2->hasTrait(TraitTag::fragment) == false) {

							auto contactGameObjectSharedPtr = m_parentScene->getGameObject(contactGameObject2->id());
							this->addTouchingObject(contactGameObjectSharedPtr.value());

						}

					}
				}

				//Now see what non-physics objects it touches
				//for (auto it = m_parentScene->getGameObjectLookup().begin(); it != m_parentScene->getGameObjectLookup().end(); ++it) {

				//	if (it->second.lock().get()->hasComponent(ComponentTypes::PHYSICS_COMPONENT) == false && this->intersectsWith(it->second.lock().get())) {

				//		this->addTouchingObject(it->second.lock());

				//	}

				//}
			}
		}
		else {

			//NOT a physics object so spin through the entire gameIndex map and see if this object intersects with
			//any other non-physics object

			//for (auto it = m_parentScene->getGameObjectLookup().begin(); it != m_parentScene->getGameObjectLookup().end(); ++it) {


			//	if (this->intersectsWith(it->second.lock().get())) {

			//		this->addTouchingObject(it->second.lock());
			//	}

			//}
		}

		

}

SDL_Color GameObject::getColor()
{

	const auto& renderComponent = getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	return renderComponent->color();

}

//bool GameObject::hasIgnoreTouchTrait(GameObject* gameObject)
//{
//
//	if (gameObject->hasTrait(TraitTag::fragment) ||
//		gameObject->hasTrait(TraitTag::fragment))
//	{
//		return true;
//	}
//	else {
//		return false;
//	}
//}

std::vector<SeenObjectDetails> GameObject::getSeenObjects()
{

	if (hasComponent(ComponentTypes::BRAIN_COMPONENT)) {
		const auto& brain = getComponent<BrainComponent>(ComponentTypes::BRAIN_COMPONENT);
		return brain->seenObjects();
	}
	else {
		return std::vector<SeenObjectDetails>();
	}

}

std::vector<std::weak_ptr<GameObject>> GameObject::getTouchingByTrait(const int trait)
{

	std::vector<std::weak_ptr<GameObject>>touchingObjects{};

	for (auto& gameObject : m_touchingGameObjects) {

		if (gameObject.second.expired() == false && gameObject.second.lock()->hasTrait(trait)) {
			touchingObjects.push_back(gameObject.second.lock());
		}

	}

	return touchingObjects;

}


std::optional<std::weak_ptr<GameObject>> GameObject::getFirstTouchingByTrait(const int trait)
{

	std::weak_ptr<GameObject>touchingObject{};

	for (auto& gameObject : m_touchingGameObjects) {

		if (gameObject.second.expired() == false && gameObject.second.lock()->hasTrait(trait)) {
			return gameObject.second.lock();
		}

	}

	return std::nullopt;

}

bool GameObject::isTouchingByTrait(const int trait)
{

	std::weak_ptr<GameObject>touchingObject{};

	for (auto& gameObject : m_touchingGameObjects) {

		if (gameObject.second.expired() == false && gameObject.second.lock()->hasTrait(trait)) {
			return true;
		}

	}

	return false;

}

bool GameObject::isTouchingByType(const std::string type)
{

	std::weak_ptr<GameObject>touchingObject{};

	for (auto& gameObject : m_touchingGameObjects) {

		if (gameObject.second.expired() == false && gameObject.second.lock()->type() == type) {
			return true;
		}

	}

	return false;

}

bool GameObject::isTouchingByName(const std::string name)
{

	std::weak_ptr<GameObject>touchingObject{};

	for (auto& gameObject : m_touchingGameObjects) {

		if (gameObject.second.expired() == false && gameObject.second.lock()->name() == name) {
			return true;
		}

	}

	return false;

}

void GameObject::_imGuiDebugObject()
{

	//if (type() == "MAIN_HUD_HOLDER") {

	//	const auto& renderComponent = getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	//	const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);


	//	ImGui::Begin("MAIN_HUD_HOLDER");

	//	ImGui::Value("Center Position X", transformComponent->getCenterPosition().x);
	//	ImGui::Value("Center Position Y", transformComponent->getCenterPosition().y);

	//	ImGui::Value("Display Position X", renderComponent->getRenderDestRect().x);
	//	ImGui::Value("Display Position Y", renderComponent->getRenderDestRect().y);


	//	ImGui::End();

	//}

	//if (type() == "HUD_INTERFACE_FRAME") {

	//	const auto& renderComponent = getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	//	const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	//	//const auto& physicsComponent = getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);


	//	ImGui::Begin("HUD_INTERFACE_FRAME Position");

	//	ImGui::Value("Center Position X", transformComponent->getCenterPosition().x);
	//	ImGui::Value("Center Position Y", transformComponent->getCenterPosition().y);

	//	/*ImGui::Value("Physics Position X", physicsComponent->position().x);
	//	ImGui::Value("Physics Position Y", physicsComponent->position().y);*/

	//	ImGui::Value("Display Position X", renderComponent->getRenderDestRect().x);
	//	ImGui::Value("Display Position Y", renderComponent->getRenderDestRect().y);

	//	

	//	ImGui::End();

	//}
	if (type() == "DRAWER_SMALL") {

		const auto& inventoryComponent = getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);


		ImGui::Begin("Small Drawer Inventory");

		for (const auto& item : inventoryComponent->items()) {

			if (item) {
				ImGui::Text(item.value()->type().c_str());
			}
		}

		ImGui::End();

	}

	if (type() == "DRAWER_MEDIUM") {

		const auto& inventoryComponent = getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);


		ImGui::Begin("Medium Drawer Inventory");

		for (const auto& item : inventoryComponent->items()) {

			if (item) {
				ImGui::Text(item.value()->type().c_str());
			}
		}

		ImGui::End();

	}

	if (type() == "BOBBY") {

		const auto& inventoryComponent = getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);


		ImGui::Begin("Bobby Inventory");

		for (const auto& item : inventoryComponent->items()) {

			if (item) {
				ImGui::Text(item.value()->type().c_str());
			}
		}

		ImGui::End();

	}
	if (type() == "HOUSE_OVERLAY") {

		const auto& renderComponent = getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
		const auto& transformComponent = getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);


		ImGui::Begin("test");

		ImGui::Text("House Adjustments");

		//Alpha
		static int alpha = renderComponent->color().a;
		ImGui::InputInt("#mouseSensitivity", &alpha, 3, 500);
		renderComponent->setColorAlpha(alpha);

		//Width
		static int width = transformComponent->getPositionRect().w;
		ImGui::InputInt("#width", &width,3, 100);

		//Height
		static int height = transformComponent->getPositionRect().h;
		ImGui::InputInt("#height", &height, 3, 100);
		transformComponent->setSize(width, height);

		//XPos
		static int xPos = transformComponent->getCenterPosition().x;
		ImGui::InputInt("#xPos", &xPos, 3, 100);

		//yPos
		static int yPos = transformComponent->getCenterPosition().y;
		ImGui::InputInt("#yPos", &yPos, 3, 100);

		transformComponent->setPosition(SDL_FPoint{(float)xPos,(float)yPos});



		ImGui::End();
	}







}