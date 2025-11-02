#include "IMGuiUtil.h"
#include "../GameConfig.h"
#include "../Renderer.h"
//#include "../game.h"
#include "../include/GameGlobals.h"


//extern std::unique_ptr<Game> game;

namespace ImGui
{
	void MobyDickInit()
	{
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

		io.IniFilename = NULL;

		ImGui::StyleColorsDark();

		auto gl_context = SDL_GL_GetCurrentContext();
		const char* glsl_version = "#version 130";

		// Setup Platform/Renderer backends
		if (GameConfig::instance().rendererType() == RendererType::OPENGL) {

			ImGui_ImplSDL2_InitForOpenGL(game->window(), gl_context);
			ImGui_ImplOpenGL3_Init(glsl_version);
		}
		else {
			ImGui_ImplSDL2_InitForSDLRenderer(game->window(), game->renderer()->sdlRenderer());
			ImGui_ImplSDLRenderer_Init(game->renderer()->sdlRenderer());
		}

		//We must load a default font
		io.Fonts->AddFontDefault();

		///temporary
		SDL_RenderSetIntegerScale(game->renderer()->sdlRenderer(), SDL_FALSE);
		/////////////


	}

	void MobyDickNewFrame()
	{

		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImGui_ImplSDL2_NewFrame();
		ImGui_ImplSDLRenderer_NewFrame();

		//Set the logical screen size
		io.DisplaySize = { static_cast<float>(game->logicalCanvasSize().x), static_cast<float>(game->logicalCanvasSize().y) };

		//IMGUI requires a 1:1 scale. Ensure that is always is here
		io.DisplayFramebufferScale = { 1, 1 };

		//Translate the mouse position to be correctly aligned with a logical canvas size vs the current screen resolution
		auto mousePosition = util::getMouseScreenPosition();
		io.MousePos = { mousePosition.x, mousePosition.y };

		ImGui::NewFrame();

	}

	void MobyDickRenderFrame()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		//IMGUI requires a 1:1 scale. Ensure that is always is here
		SDL_RenderSetScale(game->renderer()->sdlRenderer(), 1.0f, 1.0f);
		//SDL_RenderSetViewport(game->renderer()->sdlRenderer(), nullptr);
		//SDL_RenderSetClipRect(game->renderer()->sdlRenderer(), nullptr);



		//Log some values
		/*int x, y;
		SDL_GetRendererOutputSize(game->renderer()->sdlRenderer(), &x, &y);
		std::cout << ("io.DisplaySize: {} {}", io.DisplaySize.x, io.DisplaySize.y);
		std::cout << ("RendererOutputSize: {} {}", x, y);
		SDL_RenderGetLogicalSize(game->renderer()->sdlRenderer(), &x, &y);
		std::cout << ("RendererLogicalSize: {} {}", x, y);
		std::cout << ("io.Display Buffer Scale: {} {}", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);*/


		ImGui::Render();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

	}

	void MobyDickFPSFrame()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowMinSize = ImVec2{ 4,4 };
		style.WindowPadding = ImVec2{ 0,0 };

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs;
		flags |= ImGuiWindowFlags_AlwaysAutoResize;
		ImGui::Begin("FrameRate", nullptr, flags);
		
		auto frate = ImGui::GetIO().Framerate;
		ImGui::Value("FPS", frate);

		ImGui::Text("World Coords");
		ImGui::SameLine();
		ImGui::Value("X", game->getMouseScreenPosition().x);
		ImGui::SameLine();
		ImGui::Value("Y", game->getMouseScreenPosition().y);


		int dwWidth = game->logicalCanvasSize().x / 2;
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::End();

	}

	void setFont64()
	{
		//ImGui::PushFont(ImGui::font64);

	}

	void textCentered(std::string text) {
		auto windowWidth = ImGui::GetWindowSize().x;
		auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

		float diff = std::fmax((windowWidth - textWidth), 0.);
		if (diff == 0.) {
			ImGui::SetCursorPosX(10);
		}
		else {
			ImGui::SetCursorPosX(diff * 0.5f);
		}
		
		ImGui::Text("%s", text.c_str());
	}

	void setCursorToCentered(float itemToCenterSize) {
		auto windowWidth = ImGui::GetWindowSize().x;
		auto textWidth = itemToCenterSize;

		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	}

	ImGuiKey getNumKey(int keyInt)
	{
		switch (keyInt) {

		case 1:
			return ImGuiKey_1;
			break;

		case 2:
			return ImGuiKey_2;
			break;

		case 3:
			return ImGuiKey_3;
			break;

		case 4:
			return ImGuiKey_4;
			break;

		default:
			return ImGuiKey_0;
			break;

		}

	}

	void adjustCursorPosition(float x, float y)
	{
		//Display the Key letter
		ImVec2 cursorPos = ImGui::GetCursorPos();

		// Calculate the new cursor position
		ImVec2 newCursorPos = { cursorPos.x + x, cursorPos.y + y };

		// Set the cursor position to the new position
		ImGui::SetCursorPos(newCursorPos);

	}

	

}
