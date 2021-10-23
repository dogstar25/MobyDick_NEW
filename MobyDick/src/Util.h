#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <map>

#include <json/json.h>
#include <SDL2/SDL.h>
#include <box2d/box2d.h>
#include <glm/glm.hpp>

/*
Overloaded operators used throught the game
*/
static bool operator ==(SDL_Color a, SDL_Color b)
{
	return (a.r == b.r) && (a.g == b.g) && (a.b == b.b);
}

static bool operator !=(SDL_Color a, SDL_Color b)
{
	return (a.r != b.r) || (a.g != b.g) || (a.b != b.b);
}

static bool operator <(SDL_Color a, SDL_Color b)
{
	return (a.r < b.r) || (a.g < b.g) || (a.b < b.b);
}

namespace util
{
	const int generateRandomNumber(int min, int max);
	const float generateRandomNumber(float min, float max);
	const SDL_Color generateRandomColor();
	const SDL_Color generateRandomColor(SDL_Color beginRange, SDL_Color endRange);
	const float radiansToDegrees(float angleInRadians);
	const float degreesToRadians(float angleInDegrees);
	const std::string floatToString(float x, int decDigits);
	const SDL_Color JsonToColor(Json::Value JsonColor);
	float normalizeRadians(float angleInRadians);
	float normalizeDegrees(float angleInDegrees);
	glm::vec2 glNormalizeTextureCoords(glm::vec2 textureCoords, glm::vec2 textureSize);
	glm::vec4 glNormalizeColor(const SDL_Color& color);
	b2Vec2& toBox2dPoint(b2Vec2& point);
	b2Vec2& toRenderPoint(b2Vec2& point);

	b2Vec2 matchParentRotation(SDL_FPoint childPosition, SDL_FPoint parentPosition, float);

};


#endif