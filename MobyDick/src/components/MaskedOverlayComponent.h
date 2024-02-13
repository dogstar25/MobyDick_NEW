#pragma once
#include "Component.h"
#include <optional>
#include "../texture.h"
#include "../Scene.h"


class MaskedOverlayComponent : public Component
{

public:
	MaskedOverlayComponent() = default;
	MaskedOverlayComponent(Json::Value definitionJSON, Scene* parentScene);
	~MaskedOverlayComponent() = default;

	void update() override;
	void render();
	void postInit() override;

private:

	std::shared_ptr<GameObject> m_backgroundObject{};
	std::shared_ptr<GameObject> m_foregroundObject{};
	std::shared_ptr<GameObject> m_foregroundObject2{};
	std::shared_ptr<GameObject> m_maskTextureObject{};

	std::shared_ptr<Texture> m_compositeTexture{};
	std::shared_ptr<Texture> m_tempTexture{};

	SDL_FPoint m_maskPosition{};

};

