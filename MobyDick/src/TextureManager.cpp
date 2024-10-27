#include "TextureManager.h"

#include <fstream>
#include "EnumMap.h"

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

			if (GameConfig::instance().rendererType() == RendererType::SDL) {

				std::shared_ptr<SDLTexture> texture = std::static_pointer_cast<SDLTexture>(textureItem.second);

				if (texture->sdlTexture != nullptr) {
					SDL_DestroyTexture(texture->sdlTexture);
					texture->sdlTexture = nullptr;
				}
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
	std::string atlasConfigfilename = "assets/" + textureAtlas + ".json";
	std::string atlasImagefilename = "assets/" + textureAtlas + ".png";
	std::ifstream ifs(atlasConfigfilename);

	assert(ifs.fail() == false && "Opening TextureAsset file failed!!");

	std::string atlasIdStr = textureAtlas.substr(textureAtlas.size() - 2);
	int atlasIdInt = std::stoi(atlasIdStr);
	atlasIdStr = "ATLAS_" + atlasIdStr;

	ifs >> root;

	std::shared_ptr<Texture> atlasTexture;


	//Build the render specific texture object
	if (GameConfig::instance().rendererType() == RendererType::SDL) {
		
		std::shared_ptr<SDLTexture> atlasTextureSDL = std::make_shared<SDLTexture>();
		atlasTextureSDL->surface = IMG_Load(atlasImagefilename.c_str());
		atlasTextureSDL->sdlTexture = SDL_CreateTextureFromSurface(game->renderer()->sdlRenderer(), atlasTextureSDL->surface);
		atlasTexture = atlasTextureSDL;

	}
	else if (GameConfig::instance().rendererType() == RendererType::OPENGL) {

		std::shared_ptr<OpenGLTexture> atlasTextureGL = std::make_shared<OpenGLTexture>();
		atlasTextureGL->surface = IMG_Load(atlasImagefilename.c_str());

		GL_TextureIndexType textureIndex = (GL_TextureIndexType)atlasIdInt;

		GLuint textureAtlasId = static_cast<RendererGL*>(game->renderer())->getTextureId(textureIndex);
		glActiveTexture((int)textureIndex);
		glBindTexture(GL_TEXTURE_2D, textureAtlasId);
		atlasTextureGL->openglTextureIndex = textureIndex;
		static_cast<RendererGL*>(game->renderer())->prepTexture(atlasTextureGL.get());
		atlasTexture = atlasTextureGL;

	}

	//Store the atlas texture
	m_textureAtlasMap.emplace(atlasIdStr, atlasTexture);

	std::shared_ptr<Texture> texture{};

	//Store all individual textures
	for (auto itr : root["textures"])
	{
		if (GameConfig::instance().rendererType() == RendererType::SDL) {

			std::shared_ptr<SDLTexture> textureSDL = std::make_shared<SDLTexture>();
			textureSDL->sdlTexture = std::static_pointer_cast<SDLTexture>(m_textureAtlasMap[atlasIdStr])->sdlTexture;
			texture = textureSDL;

		}
		else if (GameConfig::instance().rendererType() == RendererType::OPENGL) {

			std::shared_ptr<OpenGLTexture> atlasTextureGL = std::make_shared<OpenGLTexture>();
			atlasTextureGL->openglTextureIndex = std::static_pointer_cast<OpenGLTexture>(m_textureAtlasMap[atlasIdStr])->openglTextureIndex;
			texture = atlasTextureGL;

		}

		auto quadX = itr["quadPosition"]["x"].asInt();
		auto quadY = itr["quadPosition"]["y"].asInt();
		auto quadWidth = itr["quadPosition"]["width"].asInt();
		auto quadHeight = itr["quadPosition"]["height"].asInt();

		SDL_Rect quad = { quadX , quadY, quadWidth, quadHeight };

		texture->textureAtlasQuad = std::move(quad);
		texture->applyFlip = itr["flip"].asBool();

		auto textureId = itr["id"].asString();

		//surface
		texture->surface = m_textureAtlasMap[atlasIdStr]->surface;

		m_textureMap.emplace(textureId, texture);

	}
}

void TextureManager::loadBlueprints(std::string blueprintAssets)
{

	Json::Value root;
	std::string filename = "assets/" + blueprintAssets + ".json";
	std::ifstream ifs(filename);

	assert(ifs.fail() == false && "Opening blueprintAssets file failed!!");

	ifs >> root;

	for (auto itr : root["textureBlueprints"])
	{
		//textureObject = new Texture();
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();

		auto id = itr["id"].asString();
		auto imageFilename = itr["filename"].asString();

		//Load the file
		texture->surface = IMG_Load(imageFilename.c_str());

		m_blueprintMap.emplace(id, texture);

	}

}

void TextureManager::loadFonts(std::string fontAssets)
{

	Json::Value root;
	std::string filename = "assets/" + fontAssets + ".json";
	std::ifstream ifs(filename);

	assert(ifs.fail() == false && "Opening fontAssets file failed!!");

	ifs >> root;

	for (auto itr : root["fonts"])
	{
		auto id = itr["id"].asString();
		auto imageFilename = itr["filename"].asString();
		m_fontMap.emplace(id, imageFilename);

	}

}

void TextureManager::loadCursors(std::string cursorAtlas)
{
	Json::Value root;
	std::string filename = "assets/" + cursorAtlas + ".json";
	std::ifstream ifs(filename);

	assert(ifs.fail() == false && "Opening cursorAtlas file failed!!");

	ifs >> root;

	for (auto itr : root["mouseCursors"])
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









