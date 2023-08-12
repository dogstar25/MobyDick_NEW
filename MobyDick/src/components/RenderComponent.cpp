#include "RenderComponent.h"

#include <algorithm> 

#include "../EnumMap.h"
#include "../ColorMap.h"
#include "../game.h"
#include "../BaseConstants.h"


extern std::unique_ptr<Game> game;

RenderComponent::RenderComponent(Json::Value componentJSON)
{

	m_componentType = ComponentTypes::RENDER_COMPONENT;

	if (componentJSON.isMember("color"))
	{

		if (componentJSON["color"].isObject() && componentJSON["color"].isMember("tint")) {

			m_color = game->colorMap()->toSDLColor(componentJSON["color"]["tint"].asString());

			if (componentJSON["color"].isMember("alpha")) {
				util::colorApplyAlpha(m_color, componentJSON["color"]["alpha"].asInt());
			}
			else {
				util::colorApplyAlpha(m_color, 255);
			}

		}
		//Simple color define
		else {
			m_color = game->colorMap()->toSDLColor(componentJSON["color"].asString());
			util::colorApplyAlpha(m_color, 255);
		}
	}
	else
	{

		setColor(Colors::WHITE);
		if (componentJSON.isMember("alpha")) {
			util::colorApplyAlpha(m_color, componentJSON["alpha"].asInt());
		}
		else {
			util::colorApplyAlpha(m_color, 255);
		}

	}

	m_textureId = componentJSON["textureId"].asString();

	if (componentJSON.isMember("textureBlendMode")) {
		m_textureBlendMode = static_cast<RenderBlendMode>(game->enumMap()->toEnum(componentJSON["textureBlendMode"].asString()));
	}
	else {
		m_textureBlendMode = RenderBlendMode::BLEND;
	}

	if (componentJSON.isMember("outline")) {
		m_renderOutline = true;
		m_outLineColor = game->colorMap()->toSDLColor(componentJSON["outline"]["color"].asString());
	}
	else {
		m_renderOutline = false;
	}

	m_texture = TextureManager::instance().getTexture(componentJSON["textureId"].asString());
	
	

}


RenderComponent::~RenderComponent()
{

}


void RenderComponent::postInit()
{

	//determine the parallax percent for this object
	//if it is a physics object, we cannot parallax it because it will cause weird visual issues
	//with how stuff collides

	if (parent()->hasComponent(ComponentTypes::PHYSICS_COMPONENT) == false) 
	{

		auto parallax = parent()->parentScene()->getParallax(parent()->layer());
		if (parallax.has_value()) {
			m_parallaxRate = parallax.value().rate;
		}

	}

}

void RenderComponent::update()
{

}

SDL_FRect RenderComponent::getRenderDestRect(SDL_FRect& positionRect)
{
	SDL_FRect destRect{ positionRect };

	//default to 100%
	float parallaxRate{ 0 };

	//Adjust for paralax if exists - could be negative or positive
	if (m_parallaxRate.has_value()) {

		parallaxRate = m_parallaxRate.value();
	}

	float camerAdjustX = Camera::instance().frame().x;
	float camerAdjustY = Camera::instance().frame().y;

	camerAdjustX = std::max((float)0, camerAdjustX + (camerAdjustX * parallaxRate));
	camerAdjustY = std::max((float)0, camerAdjustY + (camerAdjustY * parallaxRate));

	destRect.x -= camerAdjustX;
	destRect.y -= camerAdjustY;

	return destRect;

}

/*
Get the destination for rendering the gameObject
The end result should be a rectangle with a width and height in pixels and
an x, y position that is the top left corner of the object (for SDL render function)
*/
SDL_FRect RenderComponent::getRenderDestRect()
{
	SDL_FRect destRect;

	if (parent()->type() == "OIL_CAN") {
		int todd = 1;
	}

	const auto& transform = parent()->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);

	//Get its current position. Should be center of object
	destRect = transform->getPositionRect();

	//Adjust position based on current camera position - offset
	//Physics object always need camera adjustment to render
	if (transform->absolutePositioning() == false )
	{
		//Include application of Camera adjustment and Parallax rate

		destRect = getRenderDestRect(destRect);

	}

	return destRect;
}

/*
Get the portion of the gameObject texture to render
For animated objects, this is the portion of the texture that
represents the current frame of animation
*/
SDL_Rect* RenderComponent::getRenderTextureRect(Texture* texture)
{

	SDL_Rect* textureSrcRect=nullptr;
	const auto& animationComponent = parent()->getComponent<AnimationComponent>(ComponentTypes::ANIMATION_COMPONENT);

	if (animationComponent)
	{
		textureSrcRect = animationComponent->getCurrentAnimationTextureRect();
	}
	else {
		textureSrcRect = &texture->textureAtlasQuad;
	}

	return textureSrcRect;

}

/*
Get the actual texture to display. If this is an animated object then it will have
different textures for different animation states
*/
std::shared_ptr<Texture> RenderComponent::getRenderTexture()
{

	//SDL_Texture* texture = nullptr;
	std::shared_ptr<Texture> texture{};
	const auto& animationComponent = parent()->getComponent<AnimationComponent>(ComponentTypes::ANIMATION_COMPONENT);

	if (animationComponent)
	{
		texture = animationComponent->getCurrentAnimationTexture();
	}
	else 
	{
		texture = m_texture;
	}

	return texture;
}


SDL_Surface* RenderComponent::getRenderSurface()
{

	return m_texture->surface;

}

void RenderComponent::setColor(int red, int green, int blue, int alpha)
{
	m_color.r = red;
	m_color.g = green;
	m_color.b = blue;
	m_color.a = alpha;

}

void RenderComponent::render()
{
	//If this gameObject is anabstract object then do not display
	if (parent()->hasTrait(TraitTag::abstract) == true) {
		return;
	}

	//If this object has a lighted treatment component then skip the render
	//because the lighted treatment component will do the rendering
	if (parent()->hasComponent(ComponentTypes::LIGHTED_TREATMENT_COMPONENT)) {
		return;
	}

	if (parent()->hasTrait(TraitTag::draggable)) {
		int todd = 1;
	}

	SDL_FRect destQuad = { getRenderDestRect() };
	render(getRenderTexture().get(), m_color, destQuad, m_textureBlendMode);

}

void RenderComponent::render(SDL_FRect destQuad)
{

	render(getRenderTexture().get(), m_color, destQuad, m_textureBlendMode);


}

void RenderComponent::render(SDL_FPoint destPoint)
{

	SDL_FRect destQuad = { getRenderDestRect() };
	destQuad.x = destPoint.x;
	destQuad.y = destPoint.y;
	render(getRenderTexture().get(), m_color, destQuad, m_textureBlendMode);

}

void RenderComponent::render(Texture* texture)
{

	SDL_FRect destQuad = { getRenderDestRect() };
	render(texture, m_color, destQuad, m_textureBlendMode);

}

void RenderComponent::render(Texture* texture, SDL_Color color, RenderBlendMode textureBlendMode)
{

	SDL_FRect destQuad = { getRenderDestRect() };
	render(texture, color, destQuad, textureBlendMode);

}

void RenderComponent::render(Texture* texture, SDL_Color color, SDL_FRect destQuad, RenderBlendMode textureBlendMode)
{
	
	const auto& transform = parent()->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	const auto& physics = parent()->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
		
	//Check if this object is in the viewable area of the world
	//Add a tiles width to the camera to buffer it some
	const SDL_FRect positionRect = transform->getPositionRect();
	SDL_Rect gameObjectPosRect = { (int)positionRect.x, (int)positionRect.y, (int)positionRect.w, (int)positionRect.h };
	SDL_Rect cameraRect = { (int)Camera::instance().frame().x,
		(int)Camera::instance().frame().y,
		(int)Camera::instance().frame().w + game->worldTileSize().x,
		(int)Camera::instance().frame().h + game->worldTileSize().y };

	/*
	If this object is within the viewable are or if its absolute positioned and therefore is not dependent on the camera
	or it is being parallaxed then render it
	*/
	if (SDL_HasIntersection(&gameObjectPosRect, &cameraRect) ||
		transform->absolutePositioning() == true || m_parallaxRate.has_value() == true)
	{

		bool outline{};
		SDL_Color outlineColor{};
		float angle = transform->angle();

		//SDL_FRect destQuad = getRenderDestRect();
		SDL_Rect* textureSourceQuad = getRenderTextureRect(texture);

		//SDL Only Stuff
		if (GameConfig::instance().rendererType() == RendererType::SDL) {

			SDL_Texture* sdlTexture = texture->sdlTexture;
		}

		//Alter the render variables if there is an Overlay applied!
		if (m_displayOverlay.has_value()) {

			//Main object color
			if (m_displayOverlay.value().color.has_value()) {
				color = m_displayOverlay.value().color.value();
			}
			//Outline
			if (m_displayOverlay.value().outlined.has_value()) {
				outline = true;
				outlineColor = Colors::WHITE;
			}
			if (m_displayOverlay.value().outlineColor.has_value()) {
				outlineColor = m_displayOverlay.value().outlineColor.value();
			}
		}
		else {

			outline = m_renderOutline;
			outlineColor = m_outLineColor;
		}

		//game->renderer()->drawSprite(destQuad, color, getRenderTexture().get(), textureSourceQuad, angle, outline, outlineColor, m_textureBlendMode);

		//test
		//if (parent()->hasTrait(TraitTag::player)) {
		if (false) {



			//const std::vector< SDL_Vertex > verts =
			//{
			//	{ SDL_FPoint{ 400, 150 }, SDL_Color{ 255, 0, 0, 255 }, SDL_FPoint{ 0 }, },
			//	{ SDL_FPoint{ 200, 450 }, SDL_Color{ 0, 0, 255, 255 }, SDL_FPoint{ 0 }, },
			//	{ SDL_FPoint{ 600, 450 }, SDL_Color{ 0, 255, 0, 255 }, SDL_FPoint{ 0 }, },
			//};

			const std::vector< SDL_Vertex > verts =
			{
				{ SDL_FPoint{ 500,200 }, SDL_Color{ 255, 255, 255, 0 }, SDL_FPoint{ 0,0 }, },
				{ SDL_FPoint{ 1000,200 }, SDL_Color{ 255, 255, 255, 0 }, SDL_FPoint{ 1,0 }, },
				{ SDL_FPoint{ 500,700 }, SDL_Color{ 255, 255, 255, 0 }, SDL_FPoint{ 0,1 }, },
			};


			auto blendMode = SDL_ComposeCustomBlendMode(
				SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD, 
				SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_COLOR, SDL_BLENDOPERATION_MAXIMUM);
			//SDL_SetRenderDrawColor(game->renderer()->sdlRenderer(), 255, 255, 255, SDL_ALPHA_TRANSPARENT);
			//SDL_SetTextureBlendMode(getRenderTexture().get()->sdlTexture, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawBlendMode(game->renderer()->sdlRenderer(), blendMode);


			std::array<int, 9> indexList = { 0,1,2,2,1,3,2,4,3};

			//SDL_RenderGeometry(game->renderer()->sdlRenderer(), getRenderTexture().get()->sdlTexture, verts.data(), verts.size(), indexList.data(), (int)indexList.size());
			SDL_RenderGeometry(game->renderer()->sdlRenderer(), nullptr, verts.data(), verts.size(), nullptr, 0);
			//SDL_RenderGeometry(game->renderer()->sdlRenderer(), nullptr, verts.data(), verts.size(), nullptr, 0);


			//SDL_Color col{ 0xff,0xff,0xff,0xff };
			//std::vector<SDL_Vertex> verticies{
			//	{{300.0f,200.0f},	col,	{0.0f,0.0f}},
			//	{{800.0f,200.0f},	col,	{1.0f,0.0f}},
			//	{{300.0f,700.0f},	col,	{0.0f,1.0f}},
			//	{{800.0f,700.0f},	col,	{1.0f,1.0f}},
			//};
			//std::array<int, 6> indexList = { 0,1,2,2,1,3 };
			//SDL_RenderGeometry(game->renderer()->sdlRenderer(), nullptr, verticies.data(), (int)verticies.size(), indexList.data(), (int)indexList.size());

		}
		else {
			game->renderer()->drawSprite(parent()->layer(), destQuad, color, texture, textureSourceQuad, angle, outline, outlineColor, textureBlendMode);
		}

	}

}

void RenderComponent::renderToTexture(Texture* destTexture, GameObject* gameObectToRender, SDL_FPoint destPoint, RenderBlendMode textureBlendMode)
{
	///
	///
	/// NOTE:I dont know if this works. I used it for a while and realized it might not be the best in terms of
	/// performance. So, for now, I am changing the render target right before the render instead of
	/// this which is switching the render tagert every time this is called. this was done during the time
	/// of texture light development and i'm still learning (05/13/2023)
	/// 
	//Set the render target to the texture destination texture
	SDL_SetRenderTarget(game->renderer()->sdlRenderer(), destTexture->sdlTexture);

	const auto& renderComponent = gameObectToRender->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

	SDL_Color color = gameObectToRender->getColor();
	float angle = gameObectToRender->getAngle();

	SDL_Rect* textureSourceQuad = getRenderTextureRect(renderComponent->getRenderTexture().get());
	SDL_FRect destQuad = { destPoint.x, destPoint.y, gameObectToRender->getSize().x, gameObectToRender->getSize().y };
	

	game->renderer()->drawSprite(parent()->layer(), destQuad, color, renderComponent->getRenderTexture().get(), 
		textureSourceQuad, angle, false, SDL_Color(), textureBlendMode);

	SDL_SetRenderTarget(game->renderer()->sdlRenderer(), NULL);

}



void RenderComponent::applyDisplayOverlay(DisplayOverlay displayOverlay)
{


	m_displayOverlay = displayOverlay;

	//if (displayOverlay.color.has_value()) {
	//	m_displayOverlay.value().color = displayOverlay.color;
	//}
	//if (displayOverlay.outlined.has_value()) {
	//	m_displayOverlay.value().outlined = displayOverlay.outlined;
	//}
	//if (displayOverlay.outlineColor.has_value()) {
	//	m_displayOverlay.value().outlineColor = displayOverlay.outlineColor;
	//}


}

void RenderComponent::removeDisplayOverlay()
{
	m_displayOverlay.reset();
}

