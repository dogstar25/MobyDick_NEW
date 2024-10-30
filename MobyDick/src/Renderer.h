#pragma once

#include <vector>
#include <map>
#include <optional>
#include <memory>

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

struct PrimitivePoint{
	glm::vec2 point{};
	glm::uvec4 color{};
};

class PrimitiveLine {

public:
	//PrimitiveLine(glm::vec2 pointA, glm::vec2 pointB, glm::vec4 color) :
	//	pointA(pointA), pointB(pointB), color(color) {}
	glm::vec2 pointA{};
	glm::vec2 pointB{};
	glm::uvec4 color{};
};


class Texture;

class Renderer
{
public:

	Renderer() = default;
	~Renderer() = default;

	virtual void init(SDL_Window* window) = 0;;
	virtual bool present() = 0;
	virtual bool clear() = 0;
	virtual void setClearColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)=0;
	virtual void drawSprite(int layer, SDL_FRect quad, SDL_Color color, Texture* texture, SDL_Rect* textureSrcQuad, float angle, 
		bool outline, SDL_Color outlineColor, RenderBlendMode textureBlendMode) = 0;
	virtual void renderPrimitives(int layerIndex) = 0;

	void addLine(glm::vec2 pointA, glm::vec2 pointB, glm::uvec4 color);
	void addLine(PrimitiveLine line);
	void drawPoints(std::vector<SDL_FPoint> points, SDL_Color color = { 255,255,255,255 });

	//SDL Specific stuff
	virtual SDL_Texture* createTextureFromSurface(SDL_Surface* surface) = 0;
	virtual SDL_Renderer* sdlRenderer() = 0;
	virtual void drawBatches() {};
	virtual void renderToTexture(Texture* destTexture, GameObject* gameObectToRender, SDL_FPoint destPoint, RenderBlendMode textureBlendMode,
		bool clear = false, SDL_BlendMode customBlendMode = SDL_BLENDMODE_INVALID) {};

	//new
	virtual int setRenderTarget(Texture* targetTexture) = 0;
	virtual void resetRenderTarget() = 0;
	virtual std::shared_ptr<Texture> createEmptyTexture(int width, int height) = 0;


protected:

	void outlineObject(SDL_FRect quad, SDL_Color color);
	
	std::vector<PrimitivePoint> m_primitivePoints{};
	std::vector<PrimitiveLine> m_primitiveLines{};
	

};


