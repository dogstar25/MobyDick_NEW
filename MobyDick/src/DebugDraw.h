#ifndef DEBUGDRAW_H
#define DEBUGDRAW_H

#include <box2d/box2d.h>

class DebugDraw : public b2DebugDraw
{
public:

	static DebugDraw& instance();

	void DrawPolygon(const b2Vec2* vertices, int vertexCount, const b2HexColor& color);

	void DrawSolidPolygon(const b2Vec2* vertices, int vertexCount, const b2HexColor& color);

	void DrawCircle(const b2Vec2& center, float radius, const b2HexColor& color);

	void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2HexColor& color);

	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2HexColor& color);

	void DrawTransform(const b2Transform& xf);

	void DrawPoint(const b2Vec2& p, float size, const b2HexColor& color);

	void DrawString(int x, int y, const char* string, ...);

	void DrawAABB(b2AABB* aabb, const b2HexColor& color);
};


#endif