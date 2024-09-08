#include "MaskedOverlayComponent.h"

#include "../game.h"

extern std::unique_ptr<Game> game;

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

	m_compositeTexture = std::make_shared<SDLTexture>();
	m_tempTexture = std::make_shared<SDLTexture>();

}

void MaskedOverlayComponent::postInit()
{

	//this is very important for textures that get created on the fly
	//The textureatlasQuad has to be set to the right size.
	//See if there's a better solution to this. need a mobyDick create texture function
	m_compositeTexture->textureAtlasQuad = SDL_Rect(0, 0,
		(int)parent()->getSize().x,
		(int)parent()->getSize().y);

	m_compositeTexture->sdlTexture = SDL_CreateTexture(game->renderer()->sdlRenderer(), SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, (int)parent()->getSize().x, (int)parent()->getSize().y);

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

	///////////////////////TEMP FOR BLEND DEBUGGING////////////////////////////////////////

	static SDL_BlendFactor srcColorFactor = SDL_BLENDFACTOR_DST_COLOR;
	static SDL_BlendFactor dstColorFactor = SDL_BLENDFACTOR_DST_COLOR;
	static SDL_BlendOperation colorOperation = SDL_BLENDOPERATION_ADD;
	static SDL_BlendFactor srcAlphaFactor = SDL_BLENDFACTOR_DST_COLOR;
	static SDL_BlendFactor dstAlphaFactor = SDL_BLENDFACTOR_DST_COLOR;
	static SDL_BlendOperation alphaOperation = SDL_BLENDOPERATION_ADD;

	const SDL_BlendFactor items[] = { SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE };
	ImGui::SetNextWindowPos(ImVec2(100, 500));
	ImGui::Begin("blending");

		ImGui::InputInt("srcColorFactor", (int*) & srcColorFactor);
		ImGui::InputInt("dstColorFactor", (int*)&dstColorFactor);
		ImGui::InputInt("colorOperation", (int*)&colorOperation);
		ImGui::InputInt("srcAlphaFactor", (int*)&srcAlphaFactor);
		ImGui::InputInt("dstAlphaFactor", (int*)&dstAlphaFactor);
		ImGui::InputInt("alphaOperation", (int*)&alphaOperation);

	ImGui::End();

	SDL_BlendMode customBlendMode = SDL_ComposeCustomBlendMode(
		srcColorFactor,
		dstColorFactor,
		colorOperation,
		srcAlphaFactor,
		dstAlphaFactor,
		alphaOperation);

	////////////////////////////////////////////////////////////////////////////////////

	const auto& renderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

	//Best Mask blending for mask object to display house gradiently is
	//SDL_BlendMode customBlendModeFinal =
	//	SDL_ComposeCustomBlendMode(
	//		SDL_BLENDFACTOR_ZERO,
	//		SDL_BLENDFACTOR_ZERO,
	//		SDL_BLENDOPERATION_ADD,
	//		SDL_BLENDFACTOR_ZERO,
	//		SDL_BLENDFACTOR_ZERO,
	//		SDL_BLENDOPERATION_SUBTRACT);

	SDL_BlendMode customBlendModeFinal =
		SDL_ComposeCustomBlendMode(
			SDL_BLENDFACTOR_ZERO,
			SDL_BLENDFACTOR_ZERO,
			SDL_BLENDOPERATION_MAXIMUM,
			SDL_BLENDFACTOR_ZERO,
			SDL_BLENDFACTOR_ZERO,
			SDL_BLENDOPERATION_MINIMUM);

	//Render the overlay objects to the composite texture
	bool first = true;
	for (auto& overlayObject : m_overlayObjects) {

		bool clear = first;
		
		//Only clear on the first one
		if (overlayObject->hasState(GameObjectState::DISABLED_RENDER) == false) {

			first = false;
			game->renderer()->renderToTexture(m_compositeTexture.get(), overlayObject.get(), { 0.,0. }, RenderBlendMode::BLEND, clear);

		}
	}

	//Render the mask objects to the composite texture
	auto parentTopLeftPos = parent()->getTopLeftPosition();
	for (auto& maskObject : m_maskObjects) {

		if (maskObject->hasState(GameObjectState::ON)) {

			auto topLeftPos = maskObject->getTopLeftPosition();

			SDL_FPoint newPosition = { topLeftPos.x - parentTopLeftPos.x,  topLeftPos.y - parentTopLeftPos.y };

			game->renderer()->renderToTexture(m_compositeTexture.get(), maskObject.get(), newPosition, RenderBlendMode::CUSTOM, false, customBlendModeFinal);
		}

	}

	//Finally render the composite texture
	renderComponent->render(m_compositeTexture.get(), Colors::WHITE, RenderBlendMode::BLEND);

	///////////////////////////////////////////
	//NOTE: when rendering to a texture OR rendering a specific texture passed in as an argument
	//		the function should live with the renderer, not the renderComponent.
	//		Also, there should not be base renderer methods that have sdl or opengl specific parameters.
	//		There should be overloaded method declarations instead
	///////////////////////////////////////////


}