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
	std::shared_ptr<Texture> getTexture(std::string id);
	std::shared_ptr<Texture> getTextureAtlas(std::string id);
	std::shared_ptr<Texture> getBlueprint(std::string id);
	std::string getFont(std::string id);
	SDL_Cursor* getMouseCursor(std::string id);
	void loadTextures(std::string textureAssets);
	void loadFonts(std::string fontAssets);
	void loadCursors(std::string cursorAssets);
	void loadBlueprints(std::string blueprintAssets);

private:
	TextureManager();
	~TextureManager();

	std::map<std::string, std::shared_ptr<Texture>> m_textureAtlasMap;
	std::map<std::string, std::shared_ptr<Texture>> m_textureMap;
	std::map<std::string, std::string> m_fontMap;
	std::map<std::string, SDL_Cursor*> m_mouseCursorMap;
	std::map<std::string, std::shared_ptr<Texture>> m_blueprintMap;
	


	

};

#endif