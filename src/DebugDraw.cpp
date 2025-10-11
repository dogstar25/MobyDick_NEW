#include "DebugDraw.h"

#include "game.h"


extern std::unique_ptr<Game> game;

DebugDraw& DebugDraw::instance()
{
	static DebugDraw singletonInstance;
	return singletonInstance;
}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int vertexCount, const b2HexColor& color)
{

	DrawSolidPolygon(vertices, vertexCount, color);

}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int vertexCount, const b2HexColor& color)
{

	std::vector< SDL_FPoint> points;
	//SDL_FPoint *points = new SDL_FPoint[vertexCount + 1];

	SDL_FPoint firstPoint;
	bool firstFound = false;
	SDL_FPoint point;

	for (int i = 0; i < vertexCount; ++i) {

		b2Vec2 vector = vertices[i];

		//Adjust points based on Gamescale
		point.x = vector.x * GameConfig::instance().scaleFactor();
		point.y = vector.y * GameConfig::instance().scaleFactor();

		//Adjust position based on current camera position - offset
		point.x -= Camera::instance().frame().x;
		point.y -= Camera::instance().frame().y;

		//If this is the first point, then save it
		if (firstFound == false) {
			firstPoint = point;
			firstFound = true;
		}

		//Add point to point array
		//points[i] = point;
		points.push_back(point);
	}

	//Add the first point to the end to complete closed shape
	points.push_back(firstPoint);

	SDL_Color sdlColor = { 255,255,255,255 };
	game->renderer()->drawPoints(points, sdlColor, GameLayer::GRID_DISPLAY);

}

void DebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2HexColor& color)
{
	b2Vec2 axis;
	//DrawSolidCircle(center, radius, axis, color);
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2HexColor& color)
{
	
	//convert to propert game scale
	b2Vec2 newCenter = center;
	newCenter.x *= GameConfig::instance().scaleFactor();
	newCenter.y *= GameConfig::instance().scaleFactor();
	radius *= GameConfig::instance().scaleFactor();

	//Adjust position based on current camera position - offset
	newCenter.x -= Camera::instance().frame().x;
	newCenter.y -= Camera::instance().frame().y;

	int  sides = (int)((2 * B2_PI) * radius / 2);

	float d_a = (2 * B2_PI) / sides,
		angle = d_a;

	b2Vec2 start, end;
	end.x = radius;
	end.y = 0.0f;
	end = end + newCenter;
	for (int i = 0; i != sides; i++)
	{
		start = end;
		end.x = cos(angle) * radius;
		end.y = sin(angle) * radius;
		end = end + newCenter;
		angle += d_a;
		
		glm::vec2 startPoint = {start.x, start.y};
		glm::vec2 endPoint = { end.x, end.y };
		glm::uvec4 color = { 255,255,255,255 };
		game->renderer()->drawLine(startPoint, endPoint, color, GameLayer::GRID_DISPLAY);
	}


}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2HexColor& color)
{
	glm::vec2 startPoint = { p1.x, p1.y };
	glm::vec2 endPoint = { p2.x, p2.y };
	glm::uvec4 color2 = { 255,255,255,255 };

	std::vector< SDL_FPoint> points;
	SDL_FPoint point;

	//1
	//Adjust points based on Gamescale
	point.x = startPoint.x * GameConfig::instance().scaleFactor();
	point.y = startPoint.y * GameConfig::instance().scaleFactor();
	//Adjust position based on current camera position - offset
	point.x -= Camera::instance().frame().x;
	point.y -= Camera::instance().frame().y;
	points.push_back(point);

	//2
	//Adjust points based on Gamescale
	point.x = endPoint.x * GameConfig::instance().scaleFactor();
	point.y = endPoint.y * GameConfig::instance().scaleFactor();
	//Adjust position based on current camera position - offset
	point.x -= Camera::instance().frame().x;
	point.y -= Camera::instance().frame().y;
	points.push_back(point);

	SDL_Color sdlColor = { 255,255,255,255 };
	game->renderer()->drawPoints(points, sdlColor, GameLayer::GRID_DISPLAY);

}

void DebugDraw::DrawTransform(const b2Transform& xf)
{



}




void DebugDraw::DrawPoint(const b2Vec2& p, float size, const b2HexColor& color)
{
}

void DebugDraw::DrawString(int x, int y, const char *string, ...)
{
}

void DebugDraw::DrawAABB(b2AABB* aabb, const b2HexColor& c)
{
	SDL_FRect poly;
	poly.x = aabb->upperBound.x;
	poly.y = aabb->upperBound.y;
	poly.w = aabb->upperBound.x * 2;
	poly.h = aabb->upperBound.y * 2;
	//SDL_BlitSurface(Screen::white, &poly, Screen::screen, &poly);
}