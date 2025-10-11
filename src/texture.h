#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include "RendererGL.h"

class Texture
{
	public:
		std::string name{};
		SDL_Surface* surface{};
		SDL_Rect textureAtlasQuad{};
		bool applyFlip{ false };
};

class SDLTexture : public Texture
{
	public:
		SDL_Texture* sdlTexture{};
};

class OpenGLTexture : public Texture
{
	public:
		GLuint textureId{};
		GLuint fbo{};//frame buffer - used only if this texture is used as a target to render to
};


