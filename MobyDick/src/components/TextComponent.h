#ifndef TEXT_COMPONENT_H
#define TEXT_COMPONENT_H

#include <string>
#include <json/json.h>
#include "Component.h"
#include "../TextureManager.h"
#include "../Timer.h"

#include <SDL2/SDL.h>

class TransformComponent;
class RenderComponent;


class TextComponent : public Component
{
public:
	TextComponent() = default;
	TextComponent(std::string gameObjectId, Json::Value componentJSON, GameObject* parent);
	~TextComponent();

	void update() override;
	void setDynamicValueId(std::string dynamicValueId) { m_dynamicValueId = dynamicValueId; }

	//Accessor functions
	bool isDynamic() { return m_isDynamic; }
	bool isDebugText() { return m_isDebugText; }
	std::string fontId() { return m_fontId; }
	std::string textValue() { return m_textValue; }
	std::string debugId() { return m_debugId; }
	std::string dynamicValueId() { return m_dynamicValueId; }
	int fontSize() { return m_fontSize; }

private:

	std::shared_ptr<Texture> generateTextTexture();
	std::shared_ptr<Texture> updateDynamicTextTexture();

	void construct();

	int m_fontSize{};
	bool m_isDynamic{};
	bool m_isDebugText{};
	std::string	m_textureId{};
	std::string m_fontId{};
	std::string m_textValue{};
	std::string m_debugId{};
	std::string m_dynamicValueId{};

	static inline int m_dynamicCount{};

	//test
	TTF_Font* m_fontObject;

	Timer m_refreshTimer{};

};

#endif

