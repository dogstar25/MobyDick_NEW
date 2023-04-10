#pragma once
#include "MobyDick.h"



class IMGuiTestLevelMenu : public IMGuiItem
{
public:
	IMGuiTestLevelMenu() = default;
	IMGuiTestLevelMenu(std::string gameObjectType, b2Vec2 padding, ImVec4 backgroundColor, ImVec4 textColor, ImVec4 buttonColor, ImVec4 buttonHoverColor, ImVec4 buttonActiveColor,
		bool autoSize);
	glm::vec2 render() override;
	void sendQuitEvent();
	std::string levelInput();
	void deleteObject();
	void quit();

};