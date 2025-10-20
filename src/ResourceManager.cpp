#include "ResourceManager.h"

#include <fstream>
#include <cstring>
#include <string>
#include <cstdlib> // getenv
#include <system_error>
#include <SDL.h>

#include "game.h"

#if defined(__ANDROID__)

#include <SDL.h>
#include <SDL_system.h>
#include <android/asset_manager.h>

#elif defined(_WIN32)

#define NOMINMAX
#include <windows.h>
#include <shlobj.h>
#include <SDL_image.h>
#include <format>

#endif

extern std::unique_ptr<Game> game;

namespace mobydick {

    ////////////////////////////////////////////////////////
    //
    // Initialize the Base working directory of the game
    //
    ///////////////////////////////////////////////////////
    std::expected<void, std::string> ResourceManager::init()
    {

#ifndef GAME_WORKING_DIR

        return std::unexpected("GAME_WORKING_DIR is not defined in CMKAE project file!");

#endif 

        // Try automatically walking upward until "assets" is found
        char* basePathPtr = SDL_GetBasePath();
        std::string basePath = basePathPtr;
        free(basePathPtr);

        std::filesystem::path pathCheck = basePath;

        for (int i = 0; i < 5; ++i) // climb up to 5 levels max
        {
            std::filesystem::path testPath = pathCheck / GAME_WORKING_DIR / std::filesystem::path("assets");

            if (std::filesystem::exists(testPath))
            {
                //We found the gameTitle/assets directory, now remove the assets directory part for our working directory
                m_basePath = testPath.parent_path().string() + "/";
                SDL_Log("[Init] Base working directory is: %s", m_basePath.c_str());
                return{};
            }

            pathCheck = pathCheck.parent_path();

        }

        return std::unexpected("[Init] Warning: could not locate assets folder near %s" + basePath);
    }

    ////////////////////////////////////////////////////////
    //
    // Load and return an SDLTexture
    //
    ///////////////////////////////////////////////////////
    std::expected<std::shared_ptr<SDLTexture>, std::string> ResourceManager::getTexture(std::string textureFileName)
    {

        std::shared_ptr<SDLTexture> texture = std::make_shared<SDLTexture>();

        //Make sure base directory has been set
        if (m_basePath.empty()) {

            auto result = init();
            if(!result){

                return std::unexpected(result.error());
            }
        }

#if defined(_WIN32)

        std::filesystem::path fullTextureFileName = std::filesystem::path(m_basePath) / "assets" / textureFileName;
        std::string fullTextureFileNameStr = fullTextureFileName.string();
        texture->surface = IMG_Load(fullTextureFileNameStr.c_str());
        if (!texture->surface) {
            std::string errorMsg = std::format("getTexture: IMG_Load failed on loading {}", fullTextureFileNameStr);
            return std::unexpected(errorMsg);
        }
        texture->sdlTexture = SDL_CreateTextureFromSurface(game->renderer()->sdlRenderer(), texture->surface);

#elif defined (__ANDROID__)

        AAssetManager* mgr = SDL_AndroidGetAssetManager();
        if (!asset) {
            SDL_Log("AAsset open failed: %s", textureFileName.c_str());
            return nullptr;
        }

        AAsset* asset = AAssetManager_open(mgr, textureFileName.c_str(), AASSET_MODE_BUFFER);
        const void* buf = AAsset_getBuffer(asset);
        const size_t len = static_cast<size_t>(AAsset_getLength(asset));
        if (!buf || !len) {
            SDL_Log("Empty asset: %s", textureFileName.c_str());
            AAsset_close(asset);
            return nullptr;
        }

        SDL_RWops* rw = SDL_RWFromConstMem(buf, static_cast<int>(len));
        texture->surface = IMG_Load_RW(rw, 1);
        texture->sdlTexture = SDL_CreateTextureFromSurface(game->renderer()->sdlRenderer(), texture->surface);
        AAsset_close(asset);
        
#endif

        return texture;

    }

    ////////////////////////////////////////////////////////
    //
    // Return the requested TFFFont pointer
    //
    ///////////////////////////////////////////////////////
    std::expected<TTF_Font*, std::string> ResourceManager::getTTFFont(std::string fontFileName)
    {
        //The return object
        TTF_Font* fontObject;

        //Make sure base directory has been set
        if (m_basePath.empty()) {

            auto result = init();
            if (!result) {

                return std::unexpected(result.error());
            }
        }

#if defined(_WIN32)

        std::filesystem::path fullTTFFileName = std::filesystem::path(m_basePath) / "assets" / fontFileName;
        std::string fullTextureFileNameStr = fullTTFFileName.string();

        fontObject = TTF_OpenFont(fullTextureFileNameStr.c_str(), 32);
        if (fontObject) {

            return fontObject;
        }
        else {

            return std::unexpected("Could not open TTF file " + fullTextureFileNameStr);
        }

        

#elif defined (__ANDROID__)

#endif


        return fontObject;


    }

    ////////////////////////////////////////////////////////
    //
    // Load and return an sound Mix_Chunk
    //
    ///////////////////////////////////////////////////////
    std::expected<Mix_Chunk*, std::string> ResourceManager::getSound(std::string soundFilename)
    {

        //The return object

        Mix_Chunk* mixChunk;

        //Make sure base directory has been set
        if (m_basePath.empty()) {

            auto result = init();
            if (!result) {

                return std::unexpected(result.error());
            }
        }

#if defined(_WIN32)

        std::filesystem::path fullSoundFilename = std::filesystem::path(m_basePath) / "assets" / soundFilename;
        std::string fullSoundFilenameStr = fullSoundFilename.string();

        mixChunk = Mix_LoadWAV(fullSoundFilenameStr.c_str());
        if (!mixChunk) {

            return std::unexpected("Could not open sound file " + fullSoundFilenameStr);
        }


#elif defined (__ANDROID__)

#endif

        return mixChunk;

    }


    std::expected<Mix_Music*, std::string> ResourceManager::getMusic(std::string musicFilename)
    {

        Mix_Music* mixMusic;

        //Make sure base directory has been set
        if (m_basePath.empty()) {

            auto result = init();
            if (!result) {

                return std::unexpected(result.error());
            }
        }

#if defined(_WIN32)

        std::filesystem::path fullMusicFilename = std::filesystem::path(m_basePath) / "assets" / musicFilename;
        std::string fullMusicFilenameStr = fullMusicFilename.string();

        mixMusic = Mix_LoadMUS(fullMusicFilenameStr.c_str());
        if (!mixMusic) {

            return std::unexpected("Could not open music file " + fullMusicFilenameStr);
        }

#elif defined (__ANDROID__)

#endif

        return mixMusic;


    }

    std::expected<Json::Value, std::string> ResourceManager::getJSON(std::string filename)
    {

        //Make sure base directory has been set
        if (m_basePath.empty()) {

            auto result = init();
            if (!result) {

                return std::unexpected{result.error()};
            }
        }

        Json::Value json;

#if defined(_WIN32)

        std::filesystem::path fullFileName = std::filesystem::path(m_basePath) / std::filesystem::path("assets") / std::filesystem::path(filename);
        std::ifstream ifs(fullFileName);
        if (ifs) {
            ifs >> json;
        }
        else {
            return std::unexpected(std::string("Could not read file") + fullFileName.string() );
        }


#elif defined (__ANDROID__)


#endif
        return json;
    }


    std::expected<std::string, std::string> ResourceManager::getAssetsLocation()
    {

        //Make sure base directory has been set
        if (m_basePath.empty()) {

            auto result = init();
            if (!result) {

                return std::unexpected{ result.error() };
            }
        }

        std::filesystem::path assetsPath = std::filesystem::path(m_basePath) / std::filesystem::path("assets") ;

        return assetsPath.string();

    }

    std::expected<ImFont*, std::string> ResourceManager::loadImGuiTTFFont(ImGuiIO& imgui_io, std::string fontFileName, float size)
    {

        //The return object
        ImFont* fontObject;

        //Make sure base directory has been set
        if (!_ensureBaseDirSet())
        {
            return std::unexpected("eeor");
        }

#if defined(_WIN32)

        std::filesystem::path fullTTFFileName = std::filesystem::path(m_basePath) / "assets" / fontFileName;
        std::string fullTextureFileNameStr = fullTTFFileName.string();

        fontObject = imgui_io.Fonts->AddFontFromFileTTF(fullTextureFileNameStr.c_str(), size);
        if (!fontObject) {

            return std::unexpected("Could not load IMGUI TTF file " + fullTextureFileNameStr);
        }



#elif defined (__ANDROID__)

#endif


        return fontObject;
    }

    std::expected<Json::Value, std::string> ResourceManager::getUserPathDataJSON(std::string filename)
    {
        //Make sure base directory has been set
        if (m_basePath.empty()) {

            auto result = init();
            if (!result) {

                return std::unexpected{ result.error() };
            }
        }

        Json::Value jsonData;
        std::string writableDirUtf8;

        //std::filesystem::path fullFileName = std::filesystem::path(m_basePath) / std::filesystem::path("assets") / std::filesystem::path(filename);
        if (char* prefPathCstr = SDL_GetPrefPath("com.sinbadgames", GAME_WORKING_DIR)) {
            writableDirUtf8.assign(prefPathCstr);
            SDL_free(prefPathCstr);
        }
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

    std::expected<Json::Value, std::string> ResourceManager::saveUserPathDataJSON(Json::Value jsonData, std::string filename)
    {
        //Make sure base directory has been set
        if (m_basePath.empty()) {

            auto result = init();
            if (!result) {

                return std::unexpected{ result.error() };
            }
        }

        Json::Value json;
        std::string writableDirUtf8;

        //std::filesystem::path fullFileName = std::filesystem::path(m_basePath) / std::filesystem::path("assets") / std::filesystem::path(filename);
        if (char* prefPathCstr = SDL_GetPrefPath("com.sinbadgames", GAME_WORKING_DIR)) {
            writableDirUtf8.assign(prefPathCstr);
            SDL_free(prefPathCstr);
        }
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

            auto result = init();
            if (!result) {

                return false;
            }
        }

        return true;
    }

} // namespace md
