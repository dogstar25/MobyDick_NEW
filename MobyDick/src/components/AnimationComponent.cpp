#include "AnimationComponent.h"
#include "RenderComponent.h"
#include "../game.h"


#include "../EnumMap.h"

extern std::unique_ptr<Game> game;

AnimationComponent::AnimationComponent(Json::Value componentJSON)
{
	m_componentType = ComponentTypes::ANIMATION_COMPONENT;

	m_frameSize.x = componentJSON["frameSize"]["width"].asFloat();
	m_frameSize.y = componentJSON["frameSize"]["height"].asFloat();

	int i = 0;
	for (Json::Value animItr : componentJSON["animations"])
	{
		i++;
		std::string id = animItr["id"].asString();
		std::string textureId = animItr["textureId"].asString();

		//Initialze current animation state to the first animation in the list
		Animation animation;
		
		animation.animationMode = (AnimationMode)game->enumMap()->toEnum(animItr["animateMode"].asString());
		animation.speed = animItr["speed"].asFloat();
		animation.texture = TextureManager::instance().getTexture(textureId);
		animation.frameCount = animItr["frames"].asInt();

		//Calculate how many columns and rows this animation texture has
		int rows, columns;
		columns = (int)(animation.texture->textureAtlasQuad.w / m_frameSize.x);
		rows = (int)(animation.texture->textureAtlasQuad.h / m_frameSize.y);

		//Calculate top left corner of each animation frame
		SDL_FPoint point;
		int frameCount = 0;
		for (int rowIdx = 0; rowIdx < rows; rowIdx++)
		{
			for (int colIdx = 0; colIdx < columns; colIdx++)
			{
				point.x = animation.texture->textureAtlasQuad.x + (colIdx * m_frameSize.x);
				point.y = animation.texture->textureAtlasQuad.y + (rowIdx * m_frameSize.y);
				animation.animationFramePositions.push_back(point);
				//do not exceed the maximum number of frames that this texture holds
				frameCount++;
				if (frameCount >= animation.frameCount)
				{
					break;
				}
			}
		}


		//initialize the current texture source rect to the first frame of animation
		animation.currentTextureAnimationSrcRect = std::make_shared <SDL_Rect>();
		animation.currentTextureAnimationSrcRect->x = (int)animation.animationFramePositions.begin()[0].x;
		animation.currentTextureAnimationSrcRect->y = (int)animation.animationFramePositions.begin()[0].y;
		animation.currentTextureAnimationSrcRect->w = (int)m_frameSize.x;
		animation.currentTextureAnimationSrcRect->h = (int)m_frameSize.y;

		//Store the animation object
		m_animations[id] = animation;

	}

	//If a default state is defined then set it, otherwise just use the first animation item in the map
	if (componentJSON.isMember("defaultState")) {

		std::string defaultAnimationId = componentJSON["defaultState"].asString();
		m_defaultAnimation = m_animations[defaultAnimationId];

	}
	else {
		auto firstEntry = *m_animations.begin();
		m_defaultAnimation = firstEntry.second;
	}


}

AnimationComponent::~AnimationComponent()
{


}

void AnimationComponent::setToDefaultAnimation()
{

	m_currentAnimation = m_defaultAnimation;

}

void AnimationComponent::postInit()
{

	//Make sure we have an animation set
	if (m_currentAnimation.has_value() == false) {
		m_currentAnimation = m_defaultAnimation;
	}

}

void AnimationComponent::update()
{

	//Execute the next animation frame in the current animation
	if (m_currentAnimation.value().timer.hasMetTargetDuration())
	{

		//Increment animation frame counter and reset if it exceeds last one
		if (m_currentAnimation.value().frameCount > 1) {
			m_currentAnimation.value().currentAnimFrame += 1;
		}

		if (m_currentAnimation.value().currentAnimFrame >
			m_currentAnimation.value().frameCount - 1) {

			m_currentAnimation.value().currentAnimFrame = 0;
		}

		//build the rectangle that points to the current animation frame
		std::shared_ptr<SDL_Rect> rect = std::make_shared<SDL_Rect>();

		rect->x = (int)m_currentAnimation.value().animationFramePositions[m_currentAnimation.value().currentAnimFrame].x;
		rect->y = (int)m_currentAnimation.value().animationFramePositions[m_currentAnimation.value().currentAnimFrame].y;

		rect->w = (int)m_frameSize.x;
		rect->h = (int)m_frameSize.y;

		m_currentAnimation.value().currentTextureAnimationSrcRect = rect;
	}

	//If the animation frame has stared back over, and it should only execute once
	// then set the current animation to the default animation
	if (m_currentAnimation.value().animationMode == AnimationMode::ANIMATE_ONE_TIME &&
		m_currentAnimation.value().currentAnimFrame == 0) {

		m_currentAnimation = m_defaultAnimation;

	}

	//Should we flash?
	if (m_flashAnimation.has_value()) {

		_handleFlashing();

	}

}

void AnimationComponent::animate(std::string animationId, float speed)
{

	if (m_animations.find(animationId) != m_animations.end()) {

		m_currentAnimation = m_animations[animationId];

		//If the speed was passed in then ue it as the animation speed
		if (speed != 0.) {
			m_currentAnimation.value().speed = speed;
		}

	}


}

void AnimationComponent::_handleFlashing()
{


	const auto& renderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

	//Have we flashed the correct number of times
	if (m_flashAnimation.value().flashCount <= m_flashAnimation.value().flashTimes) {

		//Flash at the defined speed
		if (m_flashAnimation.value().speedTimer.hasMetTargetDuration()) {

			//If the flashStatus is ON, then remove the color treatment, otherwise add it
			if (m_flashAnimation.value().flashFlag == FlashFlag::flashOFF) {

				//Build a render overlay to apply to the render component
				DisplayOverlay displayOverlay{};
				displayOverlay.color = m_flashAnimation.value().flashColor;
				displayOverlay.color.value().a = m_flashAnimation.value().flashAlpha;

				renderComponent->applyDisplayOverlay(displayOverlay);

				m_flashAnimation.value().flashCount += 1;

				m_flashAnimation.value().flashFlag = FlashFlag::flashON;

			}
			else {
				renderComponent->removeDisplayOverlay();
				m_flashAnimation.value().flashFlag = FlashFlag::flashOFF;
			}

			//reset flashTimer
			m_flashAnimation.value().speedTimer.reset();
		}

	}
	else {
		renderComponent->removeDisplayOverlay();
		m_flashAnimation.reset();
	}


}



SDL_Rect* AnimationComponent::getCurrentAnimationTextureRect()
{
	//assert(m_animations.find(m_currentAnimationState) != m_animations.end() && "Animation State not found in animations collection");

	const auto& animationTextureRect = m_currentAnimation.value().currentTextureAnimationSrcRect;
	return animationTextureRect.get();

}

std::shared_ptr<Texture> AnimationComponent::getCurrentAnimationTexture()
{

	//assert(m_animations.find(m_currentAnimationState) != m_animations.end() && "Animation State not found in animations collection");
	return m_currentAnimation.value().texture;


}

void AnimationComponent::setDefaultAnimationState(std::string animationId)
{

	if (m_animations.find(animationId) != m_animations.end()) {

		m_defaultAnimation = m_animations[animationId];

	}
}

void AnimationComponent::setFlash(SDL_Color flashColor, float flashSpeed, int flashTimes)
{
	FlashAnimation flashAnimation;
	flashAnimation.flashColor = flashColor;
	flashAnimation.flashSpeed = flashSpeed;
	flashAnimation.flashTimes = flashTimes;
	flashAnimation.flashCount = 0;
	flashAnimation.speedTimer = { flashSpeed, true };
	m_flashAnimation = std::move(flashAnimation);
}



