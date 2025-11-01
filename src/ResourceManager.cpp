#include "ResourceManager.h"

#include <fstream>
#include <cstring>
#include <string>
#include <cstdlib> // getenv
#include <system_error>
#include <SDL.h>

#include "game.h"
#include <source_location>

#include <SDL_system.h>

#define NOMINMAX
#include <SDL_image.h>
#include <format>
#include "../include/GameGlobals.h"

//extern std::unique_ptr<Game> game;



namespace mobydick {

    ////////////////////////////////////////////////////////
    //
    // Initialize the Base working directory of the game
    //
    ///////////////////////////////////////////////////////
    void ResourceManager::init()
    {

        std::string basePath = "";
        
#if defined(_WIN32)
        char* basePathPtr = SDL_GetBasePath();
        m_basePath = basePath + "assets/";
        free(basePathPtr);

#elif defined (__ANDROID__)

        m_basePath = basePath;

#elif defined (__APPLE__)

        m_basePath = basePath + "Resources/assets/";

#endif

    }

    ////////////////////////////////////////////////////////
    //
    // Load and return an SDLTexture
    //
    ///////////////////////////////////////////////////////
    std::expected<std::shared_ptr<SDLTexture>, std::string> ResourceManager::getTexture(std::string textureFileName)
    {

        std::shared_ptr<SDLTexture> texture = std::make_shared<SDLTexture>();

        std::string fullTextureFileName = m_basePath + textureFileName;

        SDL_RWops* textureFile = SDL_RWFromFile(fullTextureFileName.c_str(), "rb");
        if (!textureFile) {

            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), SDL_GetError());
            return std::unexpected("ResourceManager Error");

        }

        texture->surface = IMG_Load_RW(textureFile, 1);
        if (!texture->surface) {

            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), SDL_GetError());
            return std::unexpected("ResourceManager Error");

        }

        texture->sdlTexture = SDL_CreateTextureFromSurface(game->renderer()->sdlRenderer(), texture->surface);

        return texture;

    }

    ////////////////////////////////////////////////////////
    //
    // Return the requested TFFFont pointer
    //
    ///////////////////////////////////////////////////////
    std::expected<TTF_Font*, std::string> ResourceManager::getTTFFont(std::string fontFileName, int ptSize)
    {
        //The return object
        TTF_Font* fontObject;

        std::string fullTTFFileName = m_basePath + fontFileName;

        SDL_RWops* fontFile = SDL_RWFromFile(fullTTFFileName.c_str(), "rb");
        if (!fontFile) {

            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), SDL_GetError());
            return std::unexpected("ResourceManager Error");

        }

        fontObject = TTF_OpenFontRW(fontFile, 1, ptSize);
        if (!fontObject) {

            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), SDL_GetError());
            return std::unexpected("ResourceManager Error");
        }

        return fontObject;

    }

    ////////////////////////////////////////////////////////
    //
    // Load and return an sound Mix_Chunk
    //
    ///////////////////////////////////////////////////////
    std::expected<Mix_Chunk*, std::string> ResourceManager::getSound(std::string soundFilename)
    {

        Mix_Chunk* mixChunk;

        std::string fullSoundFilename = m_basePath + soundFilename;

        SDL_RWops* soundFile = SDL_RWFromFile(fullSoundFilename.c_str(), "rb");
        if (!soundFile) {

            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), SDL_GetError());
            return std::unexpected("ResourceManager Error");

        }

        mixChunk = Mix_LoadWAV_RW(soundFile, 1);
        if (!mixChunk) {

            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), SDL_GetError());
            return std::unexpected("ResourceManager Error");
        }

        return mixChunk;

    }


    std::expected<Mix_Music*, std::string> ResourceManager::getMusic(std::string musicFilename)
    {

        Mix_Music* mixMusic;

        std::string fullMusicFilename = m_basePath + musicFilename;

        SDL_RWops* musicFile = SDL_RWFromFile(fullMusicFilename.c_str(), "rb");
        if (!musicFile) {

            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), SDL_GetError());
            return std::unexpected("ResourceManager Error");

        }

        mixMusic = Mix_LoadMUS_RW(musicFile, 1);
        if (!mixMusic) {

            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), SDL_GetError());
            return std::unexpected("ResourceManager Error");
        }

        return mixMusic;
    }

    std::expected<Json::Value, std::string> ResourceManager::getJSON(std::string filename)
    {

        Json::Value jsonData;

        std::string fullFileName = m_basePath + filename;

        SDL_RWops* jsonFile = SDL_RWFromFile(fullFileName.c_str(), "rb");
        if (!jsonFile) {

            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), SDL_GetError());
            return std::unexpected("ResourceManager Error");

        }

        size_t sz = 0;
        void* mem = SDL_LoadFile_RW(jsonFile, &sz, 1);
        if (!mem) {

            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), SDL_GetError());
            return std::unexpected("ResourceManager Error");

        }

        std::string jsonString(static_cast<char*>(mem), sz); // NUL-terminated by SDL_LoadFile_RW
        SDL_free(mem);

        Json::CharReaderBuilder builder;
        builder["collectComments"] = false;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

        std::string errs;
        const char* begin = jsonString.data();
        const char* end = begin + jsonString.size();

        if (!reader->parse(begin, end, &jsonData, &errs)) {

            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), errs.c_str());
            return std::unexpected("ResourceManager Error");

        }

        return jsonData;

    }


    std::expected<ImFont*, std::string> ResourceManager::loadImGuiTTFFont(ImGuiIO& imgui_io, std::string fontFileName, float ptSize)
    {

        //The return object
        ImFont* fontObject;


        std::string fullFileName = m_basePath + fontFileName;

        SDL_RWops* fontFile = SDL_RWFromFile(fullFileName.c_str(), "rb");
        if (!fontFile) {

            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), SDL_GetError());
            return std::unexpected("ResourceManager Error");

        }

        size_t sz = 0;
        void* data = SDL_LoadFile_RW(fontFile, &sz, 1);
        if (!data) {

            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), SDL_GetError());
            return std::unexpected("ResourceManager Error");

        }

        ImGuiIO& io = ImGui::GetIO();
        ImFontConfig cfg;
        cfg.FontDataOwnedByAtlas = true; // ImGui will free() 'data' when the atlas is destroyed

        // Use default glyph range if none provided
        const ImWchar* glyph_ranges = io.Fonts->GetGlyphRangesDefault();

        fontObject = io.Fonts->AddFontFromMemoryTTF(data, (int)sz, ptSize, &cfg, glyph_ranges);
        if (!fontObject)
        {
            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), "AddFontFromMemoryTTF failed");
            return std::unexpected("ResourceManager Error");

        }

        return fontObject;
    }

    std::expected<Json::Value, std::string> ResourceManager::getUserPathDataJSON(std::string filename)
    {

        Json::Value jsonData;
        std::string writableDirUtf8;

        char* prefPathCstr = SDL_GetPrefPath(GameConfig::instance().installGameOrg().c_str(), GameConfig::instance().installGameName().c_str());
        if (!prefPathCstr) {
            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), "AddFontFromMemoryTTF failed");
            return std::unexpected("ResourceManager Error");
        }

        writableDirUtf8.assign(prefPathCstr);
        SDL_free(prefPathCstr);

        std::filesystem::path fullFileName = std::filesystem::path{ writableDirUtf8 } / filename;
        std::filesystem::create_directories(fullFileName.parent_path());

        // 1) Try to load existing JSON
        {
            std::ifstream input(fullFileName, std::ios::binary);
            if (input) {
                Json::CharReaderBuilder builder;
                std::string errs;
                if (Json::parseFromStream(builder, input, &jsonData, &errs)) {
                    return jsonData;
                }
                // If parse fails, fall through to defaults
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                    "Settings parse failed (%s): %s",
                    fullFileName.string().c_str(), errs.c_str());
            }
        }

        // 2) Initialize defaults and save
        Json::Value blankJson();
        auto saveResult = saveUserPathDataJSON(jsonData, filename);
        if (!saveResult) {

            return std::unexpected(std::string("Could not read or initialize settings file") + fullFileName.string());
        }

        return jsonData;
    }

    std::expected<void, std::string> ResourceManager::saveUserPathDataJSON(Json::Value jsonData, std::string filename)
    {

        Json::Value json;
        std::string writableDirUtf8;

        char* prefPathCstr = SDL_GetPrefPath(GameConfig::instance().installGameOrg().c_str(), GameConfig::instance().installGameName().c_str());
        if (!prefPathCstr) {
            SDL_Log("%s %u %s", std::source_location::current().file_name(), std::source_location::current().line(), "AddFontFromMemoryTTF failed");
            return std::unexpected("ResourceManager Error");
        }

        writableDirUtf8.assign(prefPathCstr);
        SDL_free(prefPathCstr);

        std::filesystem::path fullFileName = std::filesystem::path{ writableDirUtf8 } / filename;
        std::filesystem::create_directories(fullFileName.parent_path());

		std::ofstream outputStream(fullFileName, std::ios::binary | std::ios::trunc);
		if (!outputStream) {
			return std::unexpected(std::string("Could not open for write: ") + fullFileName.string());

		}

		Json::StreamWriterBuilder writerBuilder;
		writerBuilder["indentation"] = "  ";  // compact; use "  " for pretty
		std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
		writer->write(jsonData, &outputStream);
		outputStream.flush();  // closes automatically at scope end

		if (!outputStream) {
			return std::unexpected(std::string("Write failed: ") + fullFileName.string());

		}

        return {};
    }

    bool ResourceManager::_ensureBaseDirSet()
    {

        //Make sure base directory has been set
        if (m_basePath.empty()) {

            init();
        }

        return true;
    }

}
