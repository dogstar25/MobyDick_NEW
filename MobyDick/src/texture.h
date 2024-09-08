#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "opengl/GLRenderer.h"

class Texture
{
	public:
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
		GL_TextureIndexType openglTextureIndex{};
};


