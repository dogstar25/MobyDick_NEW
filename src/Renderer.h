#pragma once

#include <vector>
#include <map>
#include <optional>
#include <memory>
#include <string>

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <box2d/box2d.h>
#include "imgui.h"
#include "BaseConstants.h"


class GameObject;



struct DisplayOverlay
{
	std::optional<SDL_Color> color;
	std::optional<bool> outlined;
	std::optional<SDL_Color> outlineColor;
};

namespace displayOverlays {

	static const DisplayOverlay outline_GREEN1{
		.color{},
		.outlined{true},
		.outlineColor{Colors::GREEN}
	};

	static const DisplayOverlay tint_RED1{
		.color{Colors::RED},
		.outlined{true},
		.outlineColor{Colors::GAS}
	};

	static const DisplayOverlay outline_CLOUD{
		.color{},
		.outlined{true},
		.outlineColor{Colors::CLOUD}
	};

	static const DisplayOverlay outline_SUBTLE_1{
		.color{},
		.outlined{true},
		.outlineColor{Colors::SMOKE}
	};

	/*
	....Add more displayOverlays here
	*/
}

enum class RenderBlendMode {

	BLEND,
	ADD,
	MULTIPLY,
	MODULATE,
	NONE,
	CUSTOM_1_MASKED_OVERLAY

};

enum class RenderTargetType {

	SCREEN,
	TEXTURE

};

class Texture;

class Renderer
{
public:

	Renderer() = default;
	~Renderer() = default;

	RenderTargetType currentRenderTargetType() { return m_currentRenderTargetType; }

	virtual void init(SDL_Window* window) = 0;;
	virtual bool present() = 0;
	virtual bool clear() = 0;
	virtual void setClearColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)=0;
	virtual void drawSprite(int layer, SDL_FRect quad, SDL_Color color, Texture* texture, SDL_Rect* textureSrcQuad, float angle, 
		bool outline, SDL_Color outlineColor, RenderBlendMode textureBlendMode) = 0;

	//SDL Specific stuff
	virtual SDL_Texture* createTextureFromSurface(SDL_Surface* surface) = 0;
	virtual SDL_Renderer* sdlRenderer() = 0;
	virtual void drawBatches() {};
	virtual void renderToTexture(Texture* destTexture, GameObject* gameObectToRender, SDL_FPoint destPoint, RenderBlendMode textureBlendMode,
		bool clear = false, SDL_BlendMode customBlendMode = SDL_BLENDMODE_INVALID) {};

	virtual void drawLine(glm::vec2 pointA, glm::vec2 pointB, glm::uvec4 color, GameLayer layer) = 0;
	virtual void drawPoints(std::vector<SDL_FPoint> points, SDL_Color color, GameLayer layer) = 0;
	virtual void drawPoint(glm::vec2 point, SDL_Color color, GameLayer layer) = 0;

	//new
	virtual int setRenderTarget(Texture* targetTexture) = 0;
	virtual void resetRenderTarget() = 0;
	virtual std::shared_ptr<Texture> createEmptyTexture(int width, int height, std::string name) = 0;


protected:

	void outlineObject(SDL_FRect quad, SDL_Color color, GameLayer layer);

	RenderTargetType m_currentRenderTargetType{ RenderTargetType ::SCREEN};

};


