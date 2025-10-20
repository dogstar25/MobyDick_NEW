#pragma once
#include <expected>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <memory>

#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL.h>
#include <SDL_system.h>
#include <json/json.h>

#include "texture.h"

namespace mobydick {

	class ResourceManager {
		public:

			static std::expected<void, std::string> init();
			static std::expected<std::shared_ptr<SDLTexture>, std::string> getTexture(std::string);
			static std::expected<TTF_Font*, std::string> getTTFFont(std::string);
			static std::expected<Mix_Chunk*, std::string> getSound(std::string);
			static std::expected<Mix_Music*, std::string> getMusic(std::string);
			static std::expected<Json::Value, std::string> getJSON(std::string);
			static std::expected<std::string, std::string> getAssetsLocation();
			static std::expected<ImFont*, std::string> loadImGuiTTFFont(ImGuiIO&, std::string, float);
			static std::expected<Json::Value, std::string> getUserPathDataJSON(std::string);
			static std::expected<Json::Value, std::string> saveUserPathDataJSON(Json::Value jsonData, std::string filename);

		private:

			inline static std::string m_basePath;

			static bool _ensureBaseDirSet();
	};

} // namespace md

