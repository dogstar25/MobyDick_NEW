#include "TextureManager.h"

#include <fstream>
#include "EnumMap.h"

#include "ResourceManager.h"
#include "game.h"

extern std::unique_ptr<Game> game;

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{

	for (auto textureItem : m_textureAtlasMap) {

		if (textureItem.second != NULL) {

			if (textureItem.second->surface != nullptr) {
				SDL_FreeSurface(textureItem.second->surface);
				textureItem.second->surface = nullptr;
			}

			std::shared_ptr<SDLTexture> texture = std::static_pointer_cast<SDLTexture>(textureItem.second);

			if (texture->sdlTexture != nullptr) {
				SDL_DestroyTexture(texture->sdlTexture);
				texture->sdlTexture = nullptr;
			}

		}
	}

	for (auto textureItem : m_blueprintMap) {

		if (textureItem.second != NULL) {

			if (textureItem.second->surface != nullptr) {
				SDL_FreeSurface(textureItem.second->surface);
				textureItem.second->surface = nullptr;
			}
		}
	}

	for (auto&& cursor : m_mouseCursorMap) {

		SDL_FreeCursor(cursor.second);
	}

	m_textureMap.clear();
	std::map<std::string, std::shared_ptr<Texture>>().swap(m_textureMap);

}


TextureManager& TextureManager::instance()
{
	static TextureManager singletonInstance;
	return singletonInstance;

}

void TextureManager::loadTextures(std::string textureAtlas)
{
	//Read file and stream it to a JSON object
	Json::Value root;
	std::string atlasConfigfilename = textureAtlas + ".json";
	std::string atlasImagefilename = textureAtlas + ".png";
	

	auto atlasConfigResult = mobydick::ResourceManager::getJSON(atlasConfigfilename);
	if (!atlasConfigResult) {

		SDL_Log("Critical error loading texture. Aborting");
		std::abort();
	}

	Json::Value atlasConfig = atlasConfigResult.value();

	//Build the texture object
	auto atlasTextureSDLResult = mobydick::ResourceManager::getTexture(atlasImagefilename);
	if (!atlasTextureSDLResult) {
		SDL_Log("Critical error loading texture. Aborting");
		std::abort();
	}

	std::shared_ptr<SDLTexture> atlasTextureSDL = atlasTextureSDLResult.value();

	//Store the atlas texture
	m_textureAtlasMap.emplace(textureAtlas, atlasTextureSDL);

	//Pointer of super class Texture that covers SDL and OpenGL
	std::shared_ptr<Texture> texture{};

	//Store all individual textures
	for (auto itr : atlasConfig["textures"])
	{
		std::shared_ptr<SDLTexture> textureSDL = std::make_shared<SDLTexture>();
		textureSDL->sdlTexture = std::static_pointer_cast<SDLTexture>(m_textureAtlasMap[textureAtlas])->sdlTexture;
		texture = textureSDL;

		auto quadX = itr["quadPosition"]["x"].asInt();
		auto quadY = itr["quadPosition"]["y"].asInt();
		auto quadWidth = itr["quadPosition"]["width"].asInt();
		auto quadHeight = itr["quadPosition"]["height"].asInt();

		SDL_Rect quad = { quadX , quadY, quadWidth, quadHeight };

		texture->textureAtlasQuad = std::move(quad);
		texture->applyFlip = itr["flip"].asBool();

		auto textureId = itr["id"].asString();

		//surface
		texture->surface = m_textureAtlasMap[textureAtlas]->surface;

		//name
		texture->name = textureAtlas;

		m_textureMap.emplace(textureId, texture);

	}
}

void TextureManager::loadBlueprints(std::string blueprintAssets)
{

	auto blueprintResult = mobydick::ResourceManager::getJSON(blueprintAssets+".json");
	if (!blueprintResult) {

		SDL_Log("Critical error loading texture. Aborting");
		std::abort();

	}

	Json::Value blueprintJSON = blueprintResult.value();

	for (auto itr : blueprintJSON["textureBlueprints"])
	{
		auto id = itr["id"].asString();
		auto textureFilename = itr["filename"].asString();

		auto textureResult = mobydick::ResourceManager::getTexture(textureFilename);
		if (!textureResult) {

			SDL_Log("Critical error loading texture. Aborting");
			std::abort();

		}
		m_blueprintMap.emplace(id, textureResult.value());

	}

}

void TextureManager::loadFonts(std::string fontAssets)
{

	auto fontsResult = mobydick::ResourceManager::getJSON(fontAssets + ".json");
	if (!fontsResult) {

		SDL_Log("Critical error loading fonts. Aborting");
		std::abort();

	}

	Json::Value fontsJSON = fontsResult.value();

	for (auto itr : fontsJSON["fonts"])
	{
		auto id = itr["id"].asString();
		auto imageFilename = itr["filename"].asString();
		m_fontMap.emplace(id, imageFilename);

	}

}

void TextureManager::loadCursors(std::string cursorAtlas)
{


	auto cursorResult = mobydick::ResourceManager::getJSON(cursorAtlas + ".json");
	if (!cursorResult) {

		SDL_Log("Critical error loading cursors. Aborting");
		std::abort();

	}

	Json::Value cursorJSON = cursorResult.value();

	for (auto itr : cursorJSON["mouseCursors"])
	{
		SDL_Point hotspot{};
		auto id = itr["id"].asString();
		if (itr.isMember("hotSpot")) {
			hotspot = { itr["hotSpot"]["x"].asInt(), itr["hotSpot"]["y"].asInt() };
		}
		std::string textureId = itr["textureId"].asString();
		auto surface = getTexture(textureId)->surface;
		auto quad = getTexture(textureId)->textureAtlasQuad;
		SDL_Surface* cursorSurface = SDL_CreateRGBSurfaceWithFormat(0, quad.w, quad.h, 32, SDL_PIXELFORMAT_RGBA32);

		SDL_SetSurfaceBlendMode(cursorSurface, SDL_BLENDMODE_BLEND);

		SDL_BlitSurface(surface, &quad, cursorSurface, NULL);

		SDL_Cursor* cursor = SDL_CreateColorCursor(cursorSurface, hotspot.x, hotspot.y);
		m_mouseCursorMap.emplace(id, cursor);

		SDL_FreeSurface(cursorSurface);

	}

}

SDL_Cursor* TextureManager::getMouseCursor(std::string id)
{

	auto iter = m_mouseCursorMap.find(id);

	if (iter != m_mouseCursorMap.end())
	{
		//fontFile = m_fontMap[id];
		return iter->second;
	}
	else //default
	{
		return m_mouseCursorMap["CURSOR_ARROW"];
	}

}

std::string TextureManager::getFont(std::string id)
{
	std::string fontFile;

	auto iter = m_fontMap.find(id);

	if (iter != m_fontMap.end())
	{
		//fontFile = m_fontMap[id];
		fontFile = iter->second;
	}
	else //default
	{
		fontFile = m_fontMap["FONT_ARIAL_REG"];
	}

	return fontFile;
}


std::shared_ptr<Texture> TextureManager::getTexture(std::string id)
{
	std::shared_ptr<Texture> textureObject;

	auto iter = m_textureMap.find(id);

	if (iter != m_textureMap.end())
	{
		textureObject = iter->second;
	}
	else
	{
		//If the texture was not found and it is being used as a blueprint for a composite or a level
		//Then we cannot use the default texture
		if (id.empty() == false) {
			assert(id.find("BLUEPRINT") != std::string::npos && "Cannot use a default texture for a blueprint texture");
		}
		
		textureObject = m_textureMap["TX_DEFAULT"];
		
	}

	return textureObject;
}


std::shared_ptr<Texture> TextureManager::getTextureAtlas(std::string id)
{
	std::shared_ptr<Texture> textureObject;

	auto iter = m_textureAtlasMap.find(id);

	if (iter != m_textureAtlasMap.end())
	{
		textureObject = iter->second;
	}
	else
	{
		//If the texture was not found and it is being used as a blueprint for a composite or a level
		//Then we cannot use the default texture
		if (id.empty() == false) {
			assert(id.find("BLUEPRINT") != std::string::npos && "Cannot use a default texture for a blueprint texture");
		}

		textureObject = m_textureAtlasMap["TX_DEFAULT"];

	}

	return textureObject;
}

std::shared_ptr<Texture> TextureManager::getBlueprint(std::string id)
{
	std::shared_ptr<Texture> textureObject;

	auto iter = m_blueprintMap.find(id);

	if (iter != m_blueprintMap.end())
	{
		textureObject = iter->second;
	}
	else
	{
		assert(false && std::string::npos && "Blueprint not found");

	}

	return textureObject;
}









