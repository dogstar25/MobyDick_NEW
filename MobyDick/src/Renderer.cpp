#include "Renderer.h"

void Renderer::outlineObject(SDL_FRect quad, SDL_Color color, GameLayer layer)
{

	//SDL_SetRenderDrawColor(m_sdlRenderer, color.r, color.g, color.b, 255);
	//SDL_RenderDrawRectF(m_sdlRenderer, &quad);

	glm::vec2 topLeftPoint(quad.x, quad.y);
	glm::vec2 topRightPoint(quad.x + quad.w, quad.y);
	glm::vec2 bottomRightPoint(quad.x + quad.w, quad.y + quad.h);
	glm::vec2 bottomLeftPoint(quad.x, quad.y + quad.h);

	glm::uvec4 lineColor(color.r, color.g, color.b, color.a);

	//top line
	drawLine(topLeftPoint, topRightPoint, lineColor, layer);

	//right side line
	drawLine(topRightPoint, bottomRightPoint, lineColor, layer);

	//bottom line
	drawLine(bottomRightPoint, bottomLeftPoint, lineColor, layer);

	//Left line
	drawLine(bottomLeftPoint, topLeftPoint, lineColor, layer);

}
