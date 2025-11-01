#include "IMGuiItem.h"
#include "../ResourceManager.h"
//#include "../game.h"
#include "../include/GameGlobals.h"

//extern std::unique_ptr<Game> game;


IMGuiItem::IMGuiItem(std::string gameObjectType, b2Vec2 padding, ImVec4 backgroundColor, ImVec4 textColor, ImVec4 buttonColor, ImVec4 buttonHoverColor, ImVec4 buttonActiveColor, bool autoSize)
{
	
	ImGuiIO& io = ImGui::GetIO();


	m_gameObjectType = gameObjectType;
	m_autoSize = autoSize;
	m_padding = padding;
	m_backgroundColor = backgroundColor;
	m_textColor = textColor;
	m_buttonColor = buttonColor;
	m_buttonHoverColor = buttonHoverColor;
	m_buttonActiveColor = buttonActiveColor;

	//Fonts - these are static so only initialize if they are null
	//First font loaded into ImGui defaults to the default font
	if (!m_normalFont) {
		auto loadFontResult = mobydick::ResourceManager::loadImGuiTTFFont(io, "fonts/DroidSans.ttf", 24.0f);
		if (loadFontResult) {
			m_normalFont = loadFontResult.value();
		}
	}

	if (!m_xSmallFont) {
		auto loadFontResult = mobydick::ResourceManager::loadImGuiTTFFont(io, "fonts/DroidSans.ttf", 8.0f);
		if (loadFontResult) {
			m_xSmallFont = loadFontResult.value();
		}

	}
	if (!m_smallFont) {
		auto loadFontResult = mobydick::ResourceManager::loadImGuiTTFFont(io, "fonts/DroidSans.ttf", 16.0f);
		if (loadFontResult) {
			m_smallFont = loadFontResult.value();
		}

	}
	if (!m_mediumFont) {
		auto loadFontResult = mobydick::ResourceManager::loadImGuiTTFFont(io, "fonts/DroidSans.ttf", 32.0f);
		if (loadFontResult) {
			m_mediumFont = loadFontResult.value();
		}

	}
	if (!m_largeFont) {
		auto loadFontResult = mobydick::ResourceManager::loadImGuiTTFFont(io, "fonts/DroidSans.ttf", 48.0f);
		if (loadFontResult) {
			m_largeFont = loadFontResult.value();
		}

	}
	if (!m_xLargeFont) {
		auto loadFontResult = mobydick::ResourceManager::loadImGuiTTFFont(io, "fonts/DroidSans.ttf", 64.0f);
		if (loadFontResult) {
			m_xLargeFont = loadFontResult.value();
		}

	}

	if (!m_xSmallGothicFont) {
		auto loadFontResult = mobydick::ResourceManager::loadImGuiTTFFont(io, "fonts/AnandaBlack.ttf", 14.0f);
		if (loadFontResult) {
			m_xSmallGothicFont = loadFontResult.value();
		}

	}
	if (!m_smallGothicFont) {
		auto loadFontResult = mobydick::ResourceManager::loadImGuiTTFFont(io, "fonts/AnandaBlack.ttf", 24.0f);
		if (loadFontResult) {
			m_smallGothicFont = loadFontResult.value();
		}

	}

}

void IMGuiItem::setWindowProperties(GameObject* parentGameObject)
{
	const auto& renderComponent = parentGameObject->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	ImGuiStyle& style = ImGui::GetStyle();

	//If this is autosized, then set the autosize flag and 
	if (m_autoSize == true) {

		m_flags |= ImGuiWindowFlags_AlwaysAutoResize;

		//We dont know the size of this window if this is the first time being displayed so Imgui has a flag for that so that the 
		//window doesnt initially appear to jump/jitter
		if (renderComponent->getRenderDestRect().w == 0) {
			ImGui::SetNextWindowPos(ImVec2{ renderComponent->getRenderDestRect().x, renderComponent->getRenderDestRect().y }, ImGuiCond_FirstUseEver);
		}
		else {
			ImGui::SetNextWindowPos(ImVec2{ renderComponent->getRenderDestRect().x, renderComponent->getRenderDestRect().y }, ImGuiCond_Appearing);
		}

	}
	//If this is NOT autosized, then set the size as defined by the component
	else {
		ImGui::SetNextWindowSize(ImVec2{ renderComponent->getRenderDestRect().w, renderComponent->getRenderDestRect().h });
		ImGui::SetNextWindowPos(ImVec2{ renderComponent->getRenderDestRect().x, renderComponent->getRenderDestRect().y });
	}

	//Set window padding
	style.WindowPadding = ImVec2{ m_padding.x, m_padding.y };

	//If this gui object is positioned based on window alignment then  we may need to adjust positioning if the size of the window changed




}

