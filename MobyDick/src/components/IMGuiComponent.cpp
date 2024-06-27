#include "IMGuiComponent.h"
#include "../game.h"
#include "../IMGui/IMGuiItem.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdlrenderer.h"
#include "../ColorMap.h"


#include <memory>

extern std::unique_ptr<Game> game;

IMGuiComponent::IMGuiComponent(Json::Value componentJSON, GameObject* parent, std::string gameObjectType, Scene* parentScene) :
	Component(ComponentTypes::IMGUI_COMPONENT, parent)
{
	auto imguiType = componentJSON["imguiType"].asString();

	ImVec4 color = { 1.0, 1.0, 1.0, 1.0 };
	if (componentJSON.isMember("color")) {
		color = util::JsonToImVec4Color(componentJSON["color"]);
	}

	auto autoSize = componentJSON["autoSize"].asBool();

	b2Vec2 padding = { componentJSON["padding"]["x"].asFloat(), componentJSON["padding"]["y"].asFloat() };
	std::string staticText = componentJSON["staticText"].asString();

	ImVec4 backgroundColor{};
	ImVec4 textColor{};
	ImVec4 buttonColor{};
	ImVec4 buttonHoverColor{};
	ImVec4 buttonActiveColor{};

	if (componentJSON.isMember("color"))
	{

		if (componentJSON["color"].isMember("background")) {
			backgroundColor = game->colorMap()->toIMGUIColor(componentJSON["color"]["background"].asString());

			if (componentJSON["color"].isMember("backgroundAlpha")) {
				util::colorApplyAlpha(backgroundColor, componentJSON["color"]["backgroundAlpha"].asInt());
			}
			else {
				util::colorApplyAlpha(backgroundColor, 255);
			}
		}

		if (componentJSON["color"].isMember("text")) {
			textColor = game->colorMap()->toIMGUIColor(componentJSON["color"]["text"].asString());
		}
		if (componentJSON["color"].isMember("button")) {
			buttonColor = game->colorMap()->toIMGUIColor(componentJSON["color"]["button"].asString());
		}
		if (componentJSON["color"].isMember("buttonHover")) {
			buttonHoverColor = game->colorMap()->toIMGUIColor(componentJSON["color"]["buttonHover"].asString());
		}
		if (componentJSON["color"].isMember("buttonActive")) {
			buttonActiveColor = game->colorMap()->toIMGUIColor(componentJSON["color"]["buttonActive"].asString());
		}
	}

	m_IMGuiItem = game->iMGUIFactory()->create(imguiType, parent, gameObjectType, padding, backgroundColor, textColor, buttonColor, buttonHoverColor, buttonActiveColor, autoSize, staticText);
}

void IMGuiComponent::update()
{
}

void IMGuiComponent::render()
{

	const auto& transform = parent()->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);

	glm::vec2 imGuiWindowSize = m_IMGuiItem->render();
	transform->setSize(b2Vec2{ imGuiWindowSize.x,imGuiWindowSize.y });

	//could we render a object here? Like a guipanel that needs to display the object like an item?
	//todd




}