#ifndef RENDERER_SDL_H
#define RENDERER_SDL_H

#include <vector>
#include <map>
#include <optional>

#include <SDL2/SDL.h>
#include <box2d/box2d.h>

#include "Renderer.h"
#include "texture.h"

class GameObject;

class RendererSDL : public Renderer
{
public:
	RendererSDL();

	void init(SDL_Window* window);

	bool present();
	virtual void setClearColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	bool clear();
	SDL_Texture* createTextureFromSurface(SDL_Surface* surface);
	void drawPoints(SDL_FPoint*, SDL_Color color);
	void drawSprite(int layer, SDL_FRect quad, SDL_Color color, Texture* texture, SDL_Rect* textureSrcQuad, float angle, 
		bool outline, SDL_Color outlineColor, RenderBlendMode textureBlendMode) override;

	void renderPrimitives(int layerIndex);
	void renderToTexture(Texture* destTexture, GameObject* gameObectToRender, SDL_FPoint destPoint, RenderBlendMode textureBlendMode,
		bool clear = false, SDL_BlendMode customBlendMode = SDL_BLENDMODE_INVALID) override;


	SDL_Renderer* sdlRenderer() {
		return m_sdlRenderer;
	}

	int setRenderTarget(Texture* targetTexture) override;
	void resetRenderTarget() override;
	std::shared_ptr<Texture> createEmptyTexture(int width, int height, std::string name) override;
	virtual bool isRenderingToScreen() override;



private:
	SDL_Renderer* m_sdlRenderer;


};


#endif
