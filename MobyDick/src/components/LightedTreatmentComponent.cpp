#include "LightedTreatmentComponent.h"
#include "../GameObject.h"
#include "../RayCastCallBack.h"

#include "../game.h"

extern std::unique_ptr<Game> game;


LightedTreatmentComponent::LightedTreatmentComponent(Json::Value componentJSON, GameObject* parent) :
	Component(ComponentTypes::LIGHTED_TREATMENT_COMPONENT, parent)
{

	m_lightCompositeTexture = game->renderer()->createEmptyTexture((int)parent->getSize().x, (int)parent->getSize().y, parent->name());

	m_lightCompositeTexture->textureAtlasQuad = SDL_Rect(0, 0,
		(int)parent->getSize().x,
		(int)parent->getSize().y);


}

LightedTreatmentComponent::~LightedTreatmentComponent()
{
	//Destroy texture
	//SDL_DestroyTexture(m_lightCompositeTexture->sdlTexture);
	//m_lightCompositeTexture->sdlTexture = nullptr;

	if (m_lightCompositeTexture->surface != nullptr) {
		SDL_FreeSurface(m_lightCompositeTexture->surface);
		m_lightCompositeTexture->surface = nullptr;
	}

	if (GameConfig::instance().rendererType() == RendererType::SDL) {

		std::shared_ptr<SDLTexture> texture = std::static_pointer_cast<SDLTexture>(m_lightCompositeTexture);

		if (texture->sdlTexture != nullptr) {
			SDL_DestroyTexture(texture->sdlTexture);
			texture->sdlTexture = nullptr;
		}
	}


}

void LightedTreatmentComponent::postInit()
{

}

void LightedTreatmentComponent::update()
{

	m_lights.clear();


	//Find all of the light objects that are within the boundaries of this lightTreatmnent Object
	const auto& lights = parent()->parentScene()->getGameObjectsByTrait(TraitTag::light_emission);
	for (auto& light : lights) {

		//If this light has a stateCOmponent, then it can potentially be turned on or off
		//so check if it is ON before adding it to th elist of lights
		if (light->hasComponent(ComponentTypes::STATE_COMPONENT) == false ||
			(light->hasComponent(ComponentTypes::STATE_COMPONENT) == true &&
				light->hasState(GameObjectState::ON))) {

			SDL_FPoint lightPoint = light->getCenterPosition();
			SDL_FRect litAreaRect = parent()->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT)->getPositionRect();
			const auto& lightComponent = light->getComponent<LightComponent>(ComponentTypes::LIGHT_COMPONENT);

			//If the center of the light is inside of the lit area then add it
			if (SDL_PointInFRect(&lightPoint, &litAreaRect)) {

				m_lights.push_back(light);
			}
			//if this light's area of lumination crosses into the lit area AND the light has direct line 
			// of sight to the lit area's center, then allow it to be added
			//This allows light from other rooms shine into adjacent rooms to a degree
			else if (parent()->intersectsWith(light.get()) && lightComponent->spreadsToOtherAreas()) {

				m_lights.push_back(light);
			}

		}

	}

}

void LightedTreatmentComponent::render()
{

	const auto& renderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	SDL_Rect tempRect{};

	//Set the target render to the composite texture for this component static_cast<SDLTexture*>(targetTexture)->sdlTexture)
	game->renderer()->setRenderTarget(m_lightCompositeTexture.get());
	game->renderer()->clear();

	//Render this object onto the composite texture
	renderComponent->render(SDL_FPoint(0, 0)); 
	SDL_FPoint baseObjectDestPosition = { renderComponent->getRenderDestRect().x, renderComponent->getRenderDestRect().y };

	//Render all of the lights to this texture also
	for (auto& light : m_lights) {

		if (light.expired() == false) {

			const auto& lightRenderComponent = light.lock()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

			//Calculate the position within the lightedTreatment Object
			//it will be an x,y that is 0,0 to the size of the lightTreatment object, however,
			// we want the center of the light to be at the light position, so the render position
			//x and y can go negative
			SDL_FRect lightDestRect = lightRenderComponent->getRenderDestRect();
			SDL_FPoint lightDestPosition = { lightDestRect.x, lightDestRect.y };
			SDL_FPoint newLightPosition = { lightDestPosition.x - baseObjectDestPosition.x,  lightDestPosition.y - baseObjectDestPosition.y };

			lightRenderComponent->render(newLightPosition);
		}
	}

	//Set the renderer to target back to the main window again
	game->renderer()->resetRenderTarget();

	//Finally render the composite texture to the main scren buffer using modulate
	renderComponent->render(m_lightCompositeTexture.get(), Colors::WHITE, RenderBlendMode::MODULATE);

}

bool LightedTreatmentComponent::_hasLineOfSightToLitArea(GameObject* lightObject)
{
	bool clearPath{true};

	b2Vec2 begin = { lightObject->getCenterPosition().x, lightObject->getCenterPosition().y};
	b2Vec2 end = { parent()->getCenterPosition().x, parent()->getCenterPosition().y};

	util::toBox2dPoint(begin);
	util::toBox2dPoint(end);

	//cast a physics raycast from the light object to the center of this lightedArea's center
	parent()->parentScene()->physicsWorld()->RayCast(&RayCastCallBack::instance(), begin, end);

	//Loop through all objects hit between the light object and the center of the lit are being checked
	for (BrainRayCastFoundItem rayHitObject : RayCastCallBack::instance().intersectionItems()) {

		//Is this a barrier and also NOT its own body and the object is not physicsdisabled
		if ((rayHitObject.gameObject->hasTrait(TraitTag::barrier) || rayHitObject.gameObject->hasState(GameObjectState::IMPASSABLE)) &&
			rayHitObject.gameObject != parent()) {
			clearPath = false;
			break;
		}
	}

	RayCastCallBack::instance().reset();

	return clearPath;

}

