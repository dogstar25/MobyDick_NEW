#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include <string>
#include <map>
#include <memory>
#include <json/json.h>

#include <memory>
#include <SDL2/SDL.h>

#include "Component.h"
#include "../texture.h"
#include "../Timer.h"

class TransformComponent;

struct Animation
{
	std::string id{};
	float   speed{ 0 };
	AnimationMode animationMode{ AnimationMode::ANIMATE_ONE_TIME };
	int     frameCount{ 0 };
	int     currentAnimFrame{ 0 };
	b2Vec2 	frameSize{ 0,0 };

	//a rectangle pointing to the animation textture of the animation frame to be displayed
	std::shared_ptr<SDL_Rect> currentTextureAnimationSrcRect;

	//Array of all x,y coordinates of the top left corner of each animation frame in the texture
	std::vector<SDL_FPoint> animationFramePositions;
	std::shared_ptr<Texture> texture{};
	Timer timer;

};


enum class FlashFlag {
	flashON,
	flashOFF
};

struct FlashAnimation
{
	SDL_Color flashColor{};
	int flashAlpha{255};
	float flashSpeed{};
	int flashTimes{};
	int flashCount{0};
	Timer speedTimer{};
	FlashFlag flashFlag { FlashFlag::flashOFF };
};

class AnimationComponent : public Component
{
public:
	AnimationComponent() {};
	AnimationComponent(Json::Value definitionJSON);
	~AnimationComponent();

	void update() override;
	void postInit() override;

	SDL_Rect* getCurrentAnimationTextureRect();
	std::shared_ptr<Texture> getCurrentAnimationTexture();
	void animate(std::string animationId, float speed=0.0);

	void setDefaultAnimationState(std::string animationId);
	void setToDefaultAnimation();
	void setFlash(SDL_Color flashColor, float flashSpeed, int flashTimes);

	std::unordered_map<std::string, Animation>& animations() { return m_animations; }

private:

	std::optional<Animation> m_currentAnimation {};
	Animation m_defaultAnimation {};
	b2Vec2 m_frameSize{};
	std::unordered_map<std::string, Animation> m_animations;
	std::optional<FlashAnimation> m_flashAnimation{};

	void _handleFlashing();
	std::string _getCurrentAnimationFromState();
};

#endif