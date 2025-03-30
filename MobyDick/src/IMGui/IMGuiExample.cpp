#pragma once
#include "IMGuiExample.h"
#include "../TextureManager.h"
#include "../game.h"
#include <SDL2/SDL.h>

extern std::unique_ptr<Game> game;

glm::vec2 IMGuiExample::render()
{

	ImGui::ShowDemoWindow();

	//Calculate the texture Atlas source rectangle (normalized)
	glm::vec2 topLeft = util::glNormalizeTextureCoords({ 37,92 }, { 4096, 4096 });
	glm::vec2 bottomRight = util::glNormalizeTextureCoords({ 68,123 }, { 4096, 4096 });

	//OpenGl Version of displaying an image
	if (GameConfig::instance().rendererType() == RendererType::OPENGL) {

		GLuint64 textureAtlasId = 7;
		ImGui::Image((GLvoid*)textureAtlasId, ImVec2(64, 64), ImVec2(.0009765, .048828), ImVec2(.01635, .0642));
	}
	else {

		//SDL2 Texture void* is the SDL_Texture*
		std::shared_ptr<Texture> texture = TextureManager::instance().getTexture("TEXTURE_ATLAS_0");
		std::shared_ptr<SDLTexture> sdlTexture = std::static_pointer_cast<SDLTexture>(texture);
		ImGui::Image((void*)(SDL_Texture*)sdlTexture.get()->sdlTexture, ImVec2(64, 64), ImVec2(.0009765, .048828), ImVec2(.01635, .0642));
	}


	glm::vec2 windowSize{ ImGui::GetWindowSize().x, ImGui::GetWindowSize().y };
	return windowSize;


}

