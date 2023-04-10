#pragma once

#include "MobyDick.h"

class BB_IMGuiFactory : public IMGuiFactory
{
public:
	BB_IMGuiFactory();

	std::shared_ptr<IMGuiItem> create(std::string iMGuiItemType, std::string gameObjectType, Scene* parentScene, b2Vec2 padding, ImVec4 backgroundColor, ImVec4 textColor,
		ImVec4 buttonColor, ImVec4 buttonHoverColor, ImVec4 buttonActiveColor, bool autoSize, std::string staticTextValue) override;

private:

};
