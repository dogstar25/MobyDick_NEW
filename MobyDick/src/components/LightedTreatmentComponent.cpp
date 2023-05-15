#include "LightedTreatmentComponent.h"
#include "../GameObject.h"
#include "../game.h"

extern std::unique_ptr<Game> game;


LightedTreatmentComponent::LightedTreatmentComponent(Json::Value componentJSON)
{

	m_componentType = ComponentTypes::LIGHTED_TREATMENT_COMPONENT;

	m_lightCompositeTexture = std::make_shared<Texture>();

}

LightedTreatmentComponent::~LightedTreatmentComponent()
{
	//Destroy texture
	SDL_DestroyTexture(m_lightCompositeTexture->sdlTexture);
	m_lightCompositeTexture->sdlTexture = nullptr;

}

void LightedTreatmentComponent::postInit()
{

	m_lightCompositeTexture->textureAtlasQuad = SDL_Rect(0, 0, parent()->getSize().x, parent()->getSize().y);

	//Make an SDL texture
	m_lightCompositeTexture->sdlTexture = SDL_CreateTexture(game->renderer()->sdlRenderer(), SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, parent()->getSize().x, parent()->getSize().y);

}

void LightedTreatmentComponent::update()
{

	m_lights.clear();

	//Find all of the light objects that are within the boundaries of this lightTreatmnent Object
	const auto& lights = parent()->parentScene()->getGameObjectsByTrait(TraitTag::light);
	for (auto& light : lights) {

		if (light->intersectsWith(parent())) {

			m_lights.push_back(light);
		}

	}



}

void LightedTreatmentComponent::render()
{

	const auto& renderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	SDL_Rect tempRect{};

	//Set the target render to the composite texture for this component
	SDL_SetRenderTarget(game->renderer()->sdlRenderer(), m_lightCompositeTexture->sdlTexture);
	SDL_SetRenderDrawColor(game->renderer()->sdlRenderer(), 0, 0, 0, 0);
	game->renderer()->clear();

	//Render this object onto the composite texture
	renderComponent->render(SDL_FPoint(0, 0)); 
	SDL_FPoint baseObjectDestPosition = { renderComponent->getRenderDestRect().x, renderComponent->getRenderDestRect().y };

	//Try using this function that may be slow because it switches the render target TO and BACK on every call
	//renderToTexture

	//Render all of the lights to this texture also
	for (auto& light : m_lights) {

		const auto& lightRenderComponent = light.lock()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

		//Calculate the position within the lightedTreatment Object
		//it will be an x,y that is 0,0 to the size of the lightTreatment object, however,
		// we want the center of the light to be at the light position, so the render position
		//x and y can go negative
		SDL_FRect lightDestRect = lightRenderComponent->getRenderDestRect();
		SDL_FPoint lightDestPosition = { lightDestRect.x, lightDestRect.y };
		SDL_FPoint newLightPosition = { lightDestPosition.x - baseObjectDestPosition.x,  lightDestPosition.y - baseObjectDestPosition.y };

		//Now position on center of light
		newLightPosition.x -= lightDestRect.w/2;
		newLightPosition.y -= lightDestRect.h/2;

		lightRenderComponent->render(newLightPosition);


	}

	//Set the renderer to target back to the main window again
	SDL_SetRenderTarget(game->renderer()->sdlRenderer(), NULL);

	//Finally render the composite texture to th main scren buffer using modulate
	renderComponent->render(m_lightCompositeTexture.get(), Colors::WHITE, RenderBlendMode::MODULATE);

}


//void LightedTreatmentComponent::render()
//{

	//SDL_Texture* outputTexture = SDL_CreateTexture(game->renderer()->sdlRenderer(), SDL_PIXELFORMAT_RGBA8888,
	//	SDL_TEXTUREACCESS_STREAMING, parent()->getSize().x, parent()->getSize().y);

	////Render the Dark onto this temporary texture object
	//SDL_SetRenderTarget(game->renderer()->sdlRenderer(), outputTexture);

	//parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT)->render();



	////Set the renderer to target the main window again
	//SDL_SetRenderTarget(game->renderer()->sdlRenderer(), nullptr);

	////Now render the new texture like normal
	//parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT)->render();







//}