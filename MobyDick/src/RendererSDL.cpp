#include "RendererSDL.h"
#include "game.h"
#include "texture.h"
#include <memory>

extern std::unique_ptr<Game> game;

RendererSDL::RendererSDL()
{
	
}

void RendererSDL::init(SDL_Window* window)
{
	//SDL_SetHint(SDL_HINT_RENDER_DRIVER, "openGL");
	SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	

	m_sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(m_sdlRenderer, 0, 0, 0, 0);


	SDL_RendererInfo rendererInfo;
	if (SDL_GetRendererInfo(m_sdlRenderer, &rendererInfo) == 0) {
		int maxTextureWidth = rendererInfo.max_texture_width;
		int maxTextureHeight = rendererInfo.max_texture_height;

		std::cout << "Maximum texture size: " << maxTextureWidth << " x " << maxTextureHeight << std::endl;
	}
	else {
		std::cerr << "Failed to get renderer info: " << SDL_GetError() << std::endl;
	}


}

bool RendererSDL::present()
{
	SDL_RenderPresent(m_sdlRenderer);
	SDL_SetRenderDrawColor(m_sdlRenderer, 0, 0, 0, 255);

	return true;
}

void RendererSDL::setClearColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{

	SDL_SetRenderDrawColor(m_sdlRenderer, r, g, b, a);

}

bool RendererSDL::clear()
{
	SDL_RenderClear(m_sdlRenderer);

	return true;
}

SDL_Texture* RendererSDL::createTextureFromSurface(SDL_Surface* surface)
{
	SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(m_sdlRenderer, surface);
	return sdlTexture;
}

void RendererSDL::drawLine(glm::vec2 pointA, glm::vec2 pointB, glm::uvec4 color, GameLayer layer)
{

	SDL_SetRenderDrawColor(m_sdlRenderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawLineF(m_sdlRenderer, pointA.x, pointA.y, pointB.x, pointB.y);

}


void RendererSDL::drawPoints(std::vector<SDL_FPoint> points, SDL_Color color, GameLayer layer)
{
	//SDL_Color saveCurrentColor = {};
	//SDL_GetRenderDrawColor(m_sdlRenderer, &saveCurrentColor.r, &saveCurrentColor.g, &saveCurrentColor.b, &saveCurrentColor.a);
	SDL_SetRenderDrawColor(m_sdlRenderer, color.r, color.b, color.g, color.a);

	SDL_RenderDrawLinesF(m_sdlRenderer, points.data(), 5);

	//SDL_SetRenderDrawColor(m_sdlRenderer, saveCurrentColor.r, saveCurrentColor.b, saveCurrentColor.g, saveCurrentColor.a);

}

void RendererSDL::drawSprite(int layer, SDL_FRect destQuad, SDL_Color color, Texture* texture, SDL_Rect* textureSrcQuad, float angle, 
	bool outline, SDL_Color outlineColor, RenderBlendMode textureBlendMode)
{

	SDLTexture* sdlTexture = static_cast<SDLTexture*>(texture);

	if (textureBlendMode == RenderBlendMode::BLEND) {
		SDL_SetTextureBlendMode(sdlTexture->sdlTexture, SDL_BLENDMODE_BLEND);
	}
	else if (textureBlendMode == RenderBlendMode::ADD) {
		SDL_SetTextureBlendMode(sdlTexture->sdlTexture, SDL_BLENDMODE_ADD);
	}
	else if (textureBlendMode == RenderBlendMode::MULTIPLY) {
		SDL_SetTextureBlendMode(sdlTexture->sdlTexture, SDL_BLENDMODE_MUL);
	}
	else if (textureBlendMode == RenderBlendMode::MODULATE) {
		SDL_SetTextureBlendMode(sdlTexture->sdlTexture, SDL_BLENDMODE_MOD);
	}
	else if (textureBlendMode == RenderBlendMode::CUSTOM_1_MASKED_OVERLAY) {

		SDL_BlendMode maskedOverlayBlendMode =
			SDL_ComposeCustomBlendMode(
				SDL_BLENDFACTOR_ZERO,
				SDL_BLENDFACTOR_ZERO,
				SDL_BLENDOPERATION_MAXIMUM,
				SDL_BLENDFACTOR_ZERO,
				SDL_BLENDFACTOR_ZERO,
				SDL_BLENDOPERATION_MINIMUM);

		SDL_SetTextureBlendMode(sdlTexture->sdlTexture, maskedOverlayBlendMode);
	}
	else{
		SDL_SetTextureBlendMode(sdlTexture->sdlTexture, SDL_BLENDMODE_NONE);
	}

	//SDL_SetTextureAlphaMod can only lower the alpha value of how the texture will render, never increate it. 
	// so a 255, will be src pixel * (255/255) which is unchanged
	SDL_SetTextureAlphaMod(sdlTexture->sdlTexture, color.a);
	SDL_SetTextureColorMod(sdlTexture->sdlTexture, color.r, color.g, color.b);

	SDL_RendererFlip flip = SDL_RendererFlip::SDL_FLIP_NONE;
	if (sdlTexture->applyFlip == true) {
		flip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;

	}

	//Render the texture
	SDL_RenderCopyExF(
		m_sdlRenderer,
		sdlTexture->sdlTexture,
		textureSrcQuad,
		&destQuad,
		angle,
		NULL,
		flip);
	
	//outline the object if defined to
	if (outline) {

		outlineObject(destQuad, outlineColor, (GameLayer)layer);
	}

}

[[deprecated("Use the setRenderTarget() and resetRenderTarget() functions instead")]]
void RendererSDL::renderToTexture(Texture* destTexture, GameObject* gameObectToRender, SDL_FPoint destPoint, RenderBlendMode textureBlendMode,
	bool clear, SDL_BlendMode customBlendMode)
{
	SDLTexture* sdlDestTexture = static_cast<SDLTexture*>(destTexture);

	//Set the render target to the texture destination texture
	SDL_SetRenderTarget(game->renderer()->sdlRenderer(), sdlDestTexture->sdlTexture);
	if (clear) {
		SDL_SetRenderDrawColor(game->renderer()->sdlRenderer(), 0, 0, 0, 0);
		game->renderer()->clear();
	}
	
	//Particle components have a special rendering method so use it directly if this is a particle component
	if (gameObectToRender->hasComponent(ComponentTypes::PARTICLE_COMPONENT)) {

		const auto& particleComponent = gameObectToRender->getComponent<ParticleComponent>(ComponentTypes::PARTICLE_COMPONENT);
		particleComponent->render();

	}
	else {
		const auto& renderComponent = gameObectToRender->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

		SDL_Color color = gameObectToRender->getColor();
		float angle = gameObectToRender->getAngle();

		SDL_Rect* textureSourceQuad = renderComponent->getRenderTextureRect(renderComponent->getRenderTexture().get());
		SDL_FRect destQuad = { destPoint.x, destPoint.y, gameObectToRender->getSize().x, gameObectToRender->getSize().y };

		game->renderer()->drawSprite(gameObectToRender->layer(), destQuad, color, renderComponent->getRenderTexture().get(),
			textureSourceQuad, angle, false, Colors::CLOUD, textureBlendMode);

	}

	//Set the render target back to the graphics card
	SDL_SetRenderTarget(game->renderer()->sdlRenderer(), NULL);

}

int RendererSDL::setRenderTarget(Texture* targetTexture)
{

	auto result = SDL_SetRenderTarget(m_sdlRenderer, static_cast<SDLTexture*>(targetTexture)->sdlTexture);

	m_currentRenderTargetType = RenderTargetType::TEXTURE;

	return result;

}

void RendererSDL::resetRenderTarget()
{

	SDL_SetRenderTarget(m_sdlRenderer, NULL);

	m_currentRenderTargetType = RenderTargetType::SCREEN;

}

std::shared_ptr<Texture> RendererSDL::createEmptyTexture(int width, int height, std::string name)
{

	std::shared_ptr<SDLTexture> texture = std::make_shared<SDLTexture>();

	texture->name = name;

	texture->sdlTexture = SDL_CreateTexture(game->renderer()->sdlRenderer(), SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, (int)width, (int)height);

	texture->textureAtlasQuad.x = 0;
	texture->textureAtlasQuad.y = 0;
	texture->textureAtlasQuad.w = width;
	texture->textureAtlasQuad.h = height;

	return texture;
}



