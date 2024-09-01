#include "AnimationComponent.h"
#include "RenderComponent.h"
#include "../game.h"


#include "../EnumMap.h"

extern std::unique_ptr<Game> game;

AnimationComponent::AnimationComponent(Json::Value componentJSON, GameObject* parent) :
	Component(ComponentTypes::ANIMATION_COMPONENT, parent)
{

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
		
		animation.id = id;
		animation.animationMode = (AnimationMode)game->enumMap()->toEnum(animItr["animateMode"].asString());
		animation.speed = animItr["speed"].asFloat();
		animation.frameCount = animItr["frames"].asInt();
		animation.texture = TextureManager::instance().getTexture(textureId);

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
	//A;so this component could be used only for flashing
	if (m_animations.empty() == false) {
		if (componentJSON.isMember("default")) {

			std::string defaultAnimationId = componentJSON["default"].asString();
			m_defaultAnimation = m_animations[defaultAnimationId];

		}
		else {
			auto firstEntry = *m_animations.begin();
			m_defaultAnimation = firstEntry.second;
		}
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

		setAnimationTexture();

		m_currentAnimation.value().timer = Timer(m_currentAnimation.value().speed, true);
	}

	//For animations that are tied to state transition, go and get the state transition duration time and 
	//calulate and override the animation frame speed using the transition duration and the number of 
	// frames of the animation
	if (parent()->hasComponent(ComponentTypes::STATE_COMPONENT)) {

		for (auto& animation : m_animations) {

			const auto& stateComponent = parent()->getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);
			auto animationTransitionDuration = stateComponent->getAnimationTransitionDuration(animation.second.id);

			if (animationTransitionDuration.has_value()) {

				float newAnimationSpeed = animationTransitionDuration.value() / animation.second.frameCount;
				animation.second.speed = newAnimationSpeed;
			}

		}

	}

}

void AnimationComponent::update()
{

	if (parent()->type() == "SKULL") {
		int todd = 1;

	}
	//This animation component may be used only for flashing
	if (m_animations.empty() == false) {

		if (parent()->hasComponent(ComponentTypes::STATE_COMPONENT)) {

			auto animationId = _getCurrentAnimationFromState();
			animate(animationId);

		}
		else if(m_currentAnimation.has_value() == false){
			animate(m_defaultAnimation.id);
		}


		//Still frame animation
		if (m_currentAnimation.value().animationMode == AnimationMode::ANIMATE_STILL_FRAME)
		{
			setAnimationTexture();
		}

		//Onetime animation
		if (m_currentAnimation.value().animationMode == AnimationMode::ANIMATE_ONE_TIME)
		{
			if (m_currentAnimation.value().timer.hasMetTargetDuration()) {

				if (m_currentAnimation.value().currentAnimFrame < m_currentAnimation.value().frameCount - 1) {

					m_currentAnimation.value().currentAnimFrame += 1;
					setAnimationTexture();

				}

			}
		}

		//Continuous animation
		if (m_currentAnimation.value().animationMode == AnimationMode::ANIMATE_CONTINUOUS)
		{
			if (m_currentAnimation.value().timer.hasMetTargetDuration()) {

				if (m_currentAnimation.value().currentAnimFrame < m_currentAnimation.value().frameCount - 1) {

					m_currentAnimation.value().currentAnimFrame += 1;
					

				}
				else {

					m_currentAnimation.value().currentAnimFrame = 0;

				}

				setAnimationTexture();

			}

		}

	}

	//Should we flash?
	if (m_flashAnimation.has_value()) {

		_handleFlashing();

	}

}

void AnimationComponent::setAnimationTexture()
{


	//build the rectangle that points to the current animation frame
	std::shared_ptr<SDL_Rect> rect = std::make_shared<SDL_Rect>();

	rect->x = (int)m_currentAnimation.value().animationFramePositions[m_currentAnimation.value().currentAnimFrame].x;
	rect->y = (int)m_currentAnimation.value().animationFramePositions[m_currentAnimation.value().currentAnimFrame].y;

	rect->w = (int)m_frameSize.x;
	rect->h = (int)m_frameSize.y;

	m_currentAnimation.value().currentTextureAnimationSrcRect = rect;

}


// Use this directly when NOT tied to a state component
void AnimationComponent::animate(std::string animationId, float speed)
{

	//If we are not already animating this particular animation then set it as our current animation 
	if ((m_currentAnimation.has_value() && m_currentAnimation.value().id != animationId 
		&& m_animations.find(animationId) != m_animations.end()) || (m_currentAnimation.has_value() == false)) {

		m_currentAnimation = m_animations[animationId];

		//If the speed was passed in then ue it as the animation speed
		if (speed != 0.) {
			m_currentAnimation.value().speed = speed;
		}

		m_currentAnimation.value().timer = Timer(m_currentAnimation.value().speed, true);

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

std::string AnimationComponent::_getCurrentAnimationFromState()
{

	const auto& stateComponent = parent()->getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);

	auto currentanimationId = stateComponent->getCurrentAnimatedState();

	if (currentanimationId.has_value()) {

		return currentanimationId.value();

	}

	return m_defaultAnimation.id;
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



