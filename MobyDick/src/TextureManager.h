#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <string>
#include <map>
#include <memory>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <box2d/box2d.h>

#include "texture.h"


class TextureManager
{

public:

	/*
	This is a singleton class using a local staic variable returned as a reference
	*/
	static TextureManager& instance();
	bool init();
	std::shared_ptr<Texture> getTexture(std::string id);
	std::string getFont(std::string id);
	SDL_Cursor* getMouseCursor(std::string id);
	bool hasTexture(std::string textureId);
	void addOrReplaceTexture(std::string textureId, std::shared_ptr<Texture> texture);
	bool load(std::string texturesAssetsFile);

private:
	TextureManager();
	~TextureManager();

	std::map<std::string, std::shared_ptr<Texture>> m_textureMap;
	std::map<std::string, std::string> m_fontMap;
	std::map<std::string, SDL_Cursor*> m_mouseCursorMap;
	


	

};

#endif