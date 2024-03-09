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

	void addOverlayObject(std::shared_ptr<GameObject> overlayObject);
	std::shared_ptr<GameObject> removeOverlayObject(std::shared_ptr<GameObject> overlayObject);

protected:

	std::vector<std::shared_ptr<GameObject>> m_overlayObjects{};
	std::vector<std::shared_ptr<GameObject>> m_maskObjects{};

	std::shared_ptr<Texture> m_compositeTexture{};
	std::shared_ptr<Texture> m_tempTexture{};

	SDL_FPoint m_maskPosition{};
	std::vector<std::string> m_maskObjectNames{};

};

