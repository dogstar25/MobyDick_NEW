#pragma once
#include <box2d/box2d.h>
#include <cstdint>

// Simple RGBA color helper if you want to convert b2HexColor to floats
struct DDColor { float r, g, b, a; };

class Box2DDebugDrawAdapter
{
public:
    Box2DDebugDrawAdapter();

    // Call this every frame (after b2World_Step) to render the world
    void draw(b2WorldId world);

    // Toggle features (these map to b2DebugDraw flags)
    void setDrawShapes(bool on);
    void setDrawJoints(bool on);
    void setDrawBounds(bool on);
    void setDrawCenters(bool on);
    void setDrawContacts(bool on);
    void setDrawContactNormals(bool on);
    void setDrawContactImpulses(bool on);
    void setDrawContactFeatures(bool on);
    void setDrawFrictionImpulses(bool on);
    void setDrawIslands(bool on);
    void setUseDrawingBounds(bool on);
    void setDrawingBounds(const b2AABB& aabb);

    // ---- Implement these with your renderer (all coords are in meters) ----
    void drawPolygon(const b2Vec2* verts, int count, b2HexColor c);
    void drawPolygon_old(const b2Vec2* verts, int count, b2HexColor c);
    void drawSolidPolygon(const b2Transform& xf, const b2Vec2* verts, int count, float radius, b2HexColor c);
    void drawCircle(const b2Vec2& center, float radius, b2HexColor c);
    void drawSolidCircle(const b2Transform& xf, float radius, b2HexColor c);
    void drawSolidCapsule(const b2Vec2& p1, const b2Vec2& p2, float radius, b2HexColor c);
    void drawSegment(const b2Vec2& p1, const b2Vec2& p2, b2HexColor c);
    void drawTransformXf(const b2Transform& xf);
    void drawPoint(const b2Vec2& p, float size, b2HexColor c);
    void drawString(const b2Vec2& p, const char* s, b2HexColor c);

    // Optional: color conversion helper
    static DDColor toColor(b2HexColor hex, float alpha = 1.0f);

private:
    // Static thunks matching b2DebugDraw signatures (route to instance via context)
    static void sDrawPolygon(const b2Vec2* v, int n, b2HexColor c, void* ctx);
    static void sDrawSolidPolygon(b2Transform xf, const b2Vec2* v, int n, float r, b2HexColor c, void* ctx);
    static void sDrawCircle(b2Vec2 center, float radius, b2HexColor c, void* ctx);
    static void sDrawSolidCircle(b2Transform xf, float radius, b2HexColor c, void* ctx);
    static void sDrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor c, void* ctx);
    static void sDrawSegment(b2Vec2 p1, b2Vec2 p2, b2HexColor c, void* ctx);
    static void sDrawTransform(b2Transform xf, void* ctx);
    static void sDrawPoint(b2Vec2 p, float size, b2HexColor c, void* ctx);
    static void sDrawString(b2Vec2 p, const char* s, b2HexColor c, void* ctx);

private:
    b2DebugDraw debugDraw_{};
};
