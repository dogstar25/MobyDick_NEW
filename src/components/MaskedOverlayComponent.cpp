#include "MaskedOverlayComponent.h"

#include "../game.h"
#include "../include/GameGlobals.h"

//extern std::unique_ptr<Game> game;

MaskedOverlayComponent::MaskedOverlayComponent(Json::Value componentJSON, GameObject* parent, Scene* parentScene) :
	Component(ComponentTypes::MASKED_OVERLAY_COMPONENT, parent)
{
	//
	// ToDo:try changing this Layer to abstract
	//

	for (Json::Value objectItr : componentJSON["overlayObjects"]) {
		m_overlayObjects.push_back( parentScene->createGameObject(objectItr.asString(), parent, -1.0F, -1.0F, 0.F, parentScene, GameLayer::MAIN));
	}

	for (Json::Value objectItr : componentJSON["maskObjects"]) {
		m_maskObjectNames.push_back(objectItr.asString());
	}

	m_compositeTexture = game->renderer()->createEmptyTexture((int)parent->getSize().x, (int)parent->getSize().y, parent->name());

	m_compositeTexture->textureAtlasQuad = SDL_Rect(0, 0,
		(int)parent->getSize().x,
		(int)parent->getSize().y);


}

void MaskedOverlayComponent::postInit()
{

	//Go and find the mask objects to be added to this component.
	//These are masks that are already created like the player mask
	for (const auto maskObjectName : m_maskObjectNames) {

		auto maskObject = parent()->parentScene()->getFirstGameObjectByName(maskObjectName);
		if (maskObject.has_value()) {
			m_maskObjects.push_back(maskObject.value());
		}

	}

	for (const auto& overlayObject : m_overlayObjects) {

		overlayObject->postInit();

	}

}

void MaskedOverlayComponent::update()
{
	//Call the update for each of the overlay objects. One might be a particle emitter or such
	for (auto& overlayObject : m_overlayObjects) {

		overlayObject->setPosition(parent()->getTopLeftPosition());
		overlayObject->update();

	}


}

void MaskedOverlayComponent::addOverlayObject(std::shared_ptr<GameObject> overlayObject)
{

	m_overlayObjects.push_back(overlayObject);

}

std::shared_ptr<GameObject> MaskedOverlayComponent::removeOverlayObject(std::shared_ptr<GameObject> overlayObject)
{

	return std::shared_ptr<GameObject>();

}

std::optional<std::shared_ptr<GameObject>> MaskedOverlayComponent::getOverlayObject(std::string gameObjectType)
{
	std::optional<std::shared_ptr<GameObject>> object{};

	for (auto& overlayObject : m_overlayObjects) {

		if (overlayObject->type() == gameObjectType) {

			return overlayObject;

		}

	}

	return object;

}

void MaskedOverlayComponent::render()
{






	const auto& renderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

	//Set the target render to the composite texture for this component static_cast<SDLTexture*>(targetTexture)->sdlTexture)
	game->renderer()->setRenderTarget(m_compositeTexture.get());
	game->renderer()->clear();

	//Render the overlay objects to the composite texture
	bool first = true;
	for (auto& overlayObject : m_overlayObjects) {

		SDL_FPoint position { 0.,0. };

		if (first) {
			first = false;
			game->renderer()->setClearColor(0,0,0,0);
			game->renderer()->clear();
		}

		if (overlayObject->hasComponent(ComponentTypes::PARTICLE_COMPONENT)) {

			const auto& particleComponent = overlayObject->getComponent<ParticleComponent>(ComponentTypes::PARTICLE_COMPONENT);
			particleComponent->render();
		}
		else {
			overlayObject->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT)->render(position, RenderBlendMode::BLEND);
		}

		
	}

	//Render the mask objects to the composite texture
	auto parentTopLeftPos = parent()->getTopLeftPosition();
	for (auto& maskObject : m_maskObjects) {

		if (maskObject->hasState(GameObjectState::ON)) {

			auto topLeftPos = maskObject->getTopLeftPosition();

			SDL_FPoint newPosition = { topLeftPos.x - parentTopLeftPos.x,  topLeftPos.y - parentTopLeftPos.y };

			maskObject->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT)->render(newPosition, RenderBlendMode::CUSTOM_1_MASKED_OVERLAY);

		}

	}

	//Reset the reder target to the screen
	game->renderer()->resetRenderTarget();

	//Finally render the composite texture
	renderComponent->render(m_compositeTexture.get(), RenderBlendMode::BLEND);

	///////////////////////////////////////////
	//NOTE: when rendering to a texture OR rendering a specific texture passed in as an argument
	//		the function should live with the renderer, not the renderComponent.
	//		Also, there should not be base renderer methods that have sdl or opengl specific parameters.
	//		There should be overloaded method declarations instead
	///////////////////////////////////////////


}