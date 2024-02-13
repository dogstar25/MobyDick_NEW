#include "MaskedOverlayComponent.h"

#include "../game.h"

extern std::unique_ptr<Game> game;

MaskedOverlayComponent::MaskedOverlayComponent(Json::Value componentJSON, Scene* parentScene) :
	Component(ComponentTypes::MASKED_OVERLAY_COMPONENT)
{
	//
	// ToDo:try changing this Layer to abstract
	//

	std::string backgroundObjectType = componentJSON["backgroundObject"].asString();
	m_backgroundObject = parentScene->createGameObject(backgroundObjectType, -1.0F, -1.0F, 0.F, parentScene, GameLayer::MAIN);

	std::string foregroundObjectType = componentJSON["foregroundObject"].asString();
	m_foregroundObject = parentScene->createGameObject(foregroundObjectType, -1.0F, -1.0F, 0.F, parentScene, GameLayer::MAIN);

	std::string maskTextureObject = componentJSON["maskTextureObject"].asString();
	m_maskTextureObject = parentScene->createGameObject(maskTextureObject, -1.0F, -1.0F, 0.F, parentScene, GameLayer::MAIN);

	m_compositeTexture = std::make_shared<Texture>();
	m_tempTexture = std::make_shared<Texture>();

	m_foregroundObject2 = parentScene->createGameObject("BOWMAN", -1.0F, -1.0F, 0.F, parentScene, GameLayer::MAIN);
	m_foregroundObject2->setSize({ 128, 128 });


}

void MaskedOverlayComponent::postInit()
{

	//this is very important for textures that get created on the fly
	//The textureatlasQuad has to be set to the right size.
	//See if there's a better solution to this. need a mobyDick create texture function
	m_tempTexture->textureAtlasQuad = SDL_Rect(0, 0,
		(int)parent()->getSize().x,
		(int)parent()->getSize().y);

	m_compositeTexture->textureAtlasQuad = SDL_Rect(0, 0,
		(int)parent()->getSize().x,
		(int)parent()->getSize().y);

	m_tempTexture->sdlTexture = SDL_CreateTexture(game->renderer()->sdlRenderer(), SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, (int)parent()->getSize().x, (int)parent()->getSize().y);

	m_compositeTexture->sdlTexture = SDL_CreateTexture(game->renderer()->sdlRenderer(), SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, (int)parent()->getSize().x, (int)parent()->getSize().y);

	//set the size of the backgfround object to be same as the main object
	m_foregroundObject->setSize({ parent()->getSize().x, parent()->getSize().y });
	m_backgroundObject->setSize({ parent()->getSize().x, parent()->getSize().y });
	m_maskTextureObject->setSize({512,256});

}

void MaskedOverlayComponent::update()
{

	m_foregroundObject->setPosition(parent()->getCenterPosition());
	m_backgroundObject->setPosition(parent()->getCenterPosition());

	m_maskPosition = parent()->parentScene()->getFirstGameObjectByTrait(TraitTag::player).value()->getCenterPosition();


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
	const auto& renderComponentFore = m_foregroundObject->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	const auto& renderComponentBack = m_backgroundObject->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	const auto& renderComponentMask = m_maskTextureObject->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

	//Best Mask blending for mask object to display house gradiently is
	SDL_BlendMode customBlendModeFinal =
		SDL_ComposeCustomBlendMode(
			SDL_BLENDFACTOR_DST_COLOR,
			SDL_BLENDFACTOR_DST_COLOR,
			SDL_BLENDOPERATION_ADD,
			SDL_BLENDFACTOR_SRC_ALPHA,
			SDL_BLENDFACTOR_ZERO,
			SDL_BLENDOPERATION_MINIMUM);

	//renderComponent->renderToTexture(m_compositeTexture.get(), m_backgroundObject.get(), { 0.,0. }, RenderBlendMode::CUSTOM, true, customBlendMode);
	//Render the foreground to the composite texture
	renderComponent->renderToTexture(m_compositeTexture.get(), m_foregroundObject.get(), { 0.,0. }, RenderBlendMode::BLEND, true, customBlendMode);
	
	//test
	renderComponent->renderToTexture(m_compositeTexture.get(), m_foregroundObject2.get(), { 900.,650. }, RenderBlendMode::BLEND, false, customBlendMode);

	renderComponent->renderToTexture(m_compositeTexture.get(), m_maskTextureObject.get(), { 450.,650.0 }, RenderBlendMode::CUSTOM, false, customBlendModeFinal);

	//test
	renderComponent->renderToTexture(m_compositeTexture.get(), m_maskTextureObject.get(), { 650.,750.0 }, RenderBlendMode::CUSTOM, false, customBlendModeFinal);

	//Finally render the composite texture to th main scren buffer using modulate
	renderComponent->render(m_compositeTexture.get(), Colors::WHITE, RenderBlendMode::BLEND);


}