#include "Box2DDebugDraw.h"
#include "game.h"

extern std::unique_ptr<Game> game;

Box2DDebugDrawAdapter::Box2DDebugDrawAdapter()
{
    // Start with defaults, then override the callbacks you care about.
    debugDraw_ = b2DefaultDebugDraw(); // zero-inits and sets sensible defaults. :contentReference[oaicite:1]{index=1}

    // Route callbacks through static thunks (context = this)
    debugDraw_.DrawPolygonFcn = &Box2DDebugDrawAdapter::sDrawPolygon;
    debugDraw_.DrawSolidPolygonFcn = &Box2DDebugDrawAdapter::sDrawSolidPolygon;
    debugDraw_.DrawCircleFcn = &Box2DDebugDrawAdapter::sDrawCircle;
    debugDraw_.DrawSolidCircleFcn = &Box2DDebugDrawAdapter::sDrawSolidCircle;
    debugDraw_.DrawSolidCapsuleFcn = &Box2DDebugDrawAdapter::sDrawSolidCapsule;
    debugDraw_.DrawSegmentFcn = &Box2DDebugDrawAdapter::sDrawSegment;
    debugDraw_.DrawTransformFcn = &Box2DDebugDrawAdapter::sDrawTransform;
    debugDraw_.DrawPointFcn = &Box2DDebugDrawAdapter::sDrawPoint;
    debugDraw_.DrawStringFcn = &Box2DDebugDrawAdapter::sDrawString;

    debugDraw_.context = this;

    // Enable common views by default
    debugDraw_.drawShapes = false;
    debugDraw_.drawJoints = false;
    debugDraw_.drawBounds = false;
    debugDraw_.drawMass = false;
    debugDraw_.drawContacts = false;
}

void Box2DDebugDrawAdapter::draw(b2WorldId world)
{
    // Single call that asks Box2D to traverse and invoke our callbacks. :contentReference[oaicite:2]{index=2}
    b2World_Draw(world, &debugDraw_);
}

// ------- Flag setters -------
void Box2DDebugDrawAdapter::setDrawShapes(bool on) { debugDraw_.drawShapes = on; }
void Box2DDebugDrawAdapter::setDrawJoints(bool on) { debugDraw_.drawJoints = on; }
void Box2DDebugDrawAdapter::setDrawBounds(bool on) { debugDraw_.drawBounds = on; }
void Box2DDebugDrawAdapter::setDrawCenters(bool on) { debugDraw_.drawMass = on; }
void Box2DDebugDrawAdapter::setDrawContacts(bool on) { debugDraw_.drawContacts = on; }
void Box2DDebugDrawAdapter::setDrawContactNormals(bool on) { debugDraw_.drawContactNormals = on; }
void Box2DDebugDrawAdapter::setDrawContactImpulses(bool on) { debugDraw_.drawContactImpulses = on; }
void Box2DDebugDrawAdapter::setDrawContactFeatures(bool on) { debugDraw_.drawContactFeatures = on; }
void Box2DDebugDrawAdapter::setDrawFrictionImpulses(bool on) { debugDraw_.drawFrictionImpulses = on; }
void Box2DDebugDrawAdapter::setDrawIslands(bool on) { debugDraw_.drawIslands = on; }
void Box2DDebugDrawAdapter::setUseDrawingBounds(bool on) { debugDraw_.useDrawingBounds = on; }
void Box2DDebugDrawAdapter::setDrawingBounds(const b2AABB& aabb) { debugDraw_.drawingBounds = aabb; }

// ------- Static thunks -> instance methods -------
void Box2DDebugDrawAdapter::sDrawPolygon(const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context)
{
    static_cast<Box2DDebugDrawAdapter*>(context)->drawPolygon(vertices, vertexCount, color);
}

void Box2DDebugDrawAdapter::sDrawSolidPolygon(b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color,
    void* context)
{
    static_cast<Box2DDebugDrawAdapter*>(context)->drawSolidPolygon(transform, vertices, vertexCount, radius, color);
}

void Box2DDebugDrawAdapter::sDrawCircle(b2Vec2 center, float radius, b2HexColor c, void* ctx)
{
    static_cast<Box2DDebugDrawAdapter*>(ctx)->drawCircle(center, radius, c);
}

void Box2DDebugDrawAdapter::sDrawSolidCircle(b2Transform xf, float radius, b2HexColor c, void* ctx)
{
    static_cast<Box2DDebugDrawAdapter*>(ctx)->drawSolidCircle(xf, radius, c);
}

void Box2DDebugDrawAdapter::sDrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor c, void* ctx)
{
    static_cast<Box2DDebugDrawAdapter*>(ctx)->drawSolidCapsule(p1, p2, radius, c);
}

void Box2DDebugDrawAdapter::sDrawSegment(b2Vec2 p1, b2Vec2 p2, b2HexColor c, void* ctx)
{
    static_cast<Box2DDebugDrawAdapter*>(ctx)->drawSegment(p1, p2, c);
}

void Box2DDebugDrawAdapter::sDrawTransform(b2Transform xf, void* ctx)
{
    static_cast<Box2DDebugDrawAdapter*>(ctx)->drawTransformXf(xf);
}

void Box2DDebugDrawAdapter::sDrawPoint(b2Vec2 p, float size, b2HexColor c, void* ctx)
{
    static_cast<Box2DDebugDrawAdapter*>(ctx)->drawPoint(p, size, c);
}

void Box2DDebugDrawAdapter::sDrawString(b2Vec2 p, const char* s, b2HexColor c, void* ctx)
{
    static_cast<Box2DDebugDrawAdapter*>(ctx)->drawString(p, s, c);
}

void Box2DDebugDrawAdapter::drawPolygon(const b2Vec2* vertices, int vertexCount, b2HexColor colorHex)
{

    std::vector< SDL_FPoint> points;
    //SDL_FPoint *points = new SDL_FPoint[vertexCount + 1];

    SDL_FPoint firstPoint;
    bool firstFound = false;
    SDL_FPoint point;

    for (int i = 0; i < vertexCount; ++i) {

        b2Vec2 vector = vertices[i];

        //Adjust points based on Gamescale
        point.x = vector.x * GameConfig::instance().scaleFactor() - Camera::instance().frame().x;
        point.y = vector.y * GameConfig::instance().scaleFactor() - Camera::instance().frame().y;

        //Adjust position based on current camera position - offset
        //point.x -= Camera::instance().frame().x;
        //point.y -= Camera::instance().frame().y;

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


void Box2DDebugDrawAdapter::drawSolidPolygon(const b2Transform& xf, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor c)
{
    if (vertexCount < 2 || vertices == nullptr) return;

    const float     S = GameConfig::instance().scaleFactor(); // e.g., 100 px/m
    const SDL_FRect cam = Camera::instance().frame();           // screen-space offset

    std::vector<SDL_FPoint> pts;
    pts.reserve(static_cast<size_t>(vertexCount) + 1);

    for (int i = 0; i < vertexCount; ++i)
    {
        const b2Vec2& v = vertices[i]; // local (shape space)

        // local -> world: w = xf.p + R * v
        b2Vec2 w;
        w.x = xf.q.c * v.x - xf.q.s * v.y + xf.p.x;
        w.y = xf.q.s * v.x + xf.q.c * v.y + xf.p.y;

        // world -> screen
        SDL_FPoint p;
        p.x = w.x * S - cam.x;
        p.y = w.y * S - cam.y;

        pts.push_back(p);
    }

    // Close the polygon
    pts.push_back(pts.front());

    // Convert Box2D 0xRRGGBB to SDL color (opaque)
    SDL_Color col;
    col.r = 255;
    col.g = 255;
    col.b = 255;
    col.a = 255;

    // Draw outline (or call your fill first, then outline if available)
    game->renderer()->drawPoints(pts, col, GameLayer::GRID_DISPLAY);
}

void Box2DDebugDrawAdapter::drawCircle(const b2Vec2& center, float radius, b2HexColor colorHex)
{
    // --- Camera & scale ---
    const float S =  GameConfig::instance().scaleFactor();
    const SDL_FRect cam = Camera::instance().frame();

    // Convert to screen-space
    b2Vec2 c = { center.x * S - cam.x, center.y * S - cam.y };
    float   r = radius * S;

    // Convert Box2D 0xRRGGBB to SDL color (opaque)
    SDL_Color sdlColor;
    sdlColor.r = (colorHex >> 16) & 0xFF;
    sdlColor.g = (colorHex >> 8) & 0xFF;
    sdlColor.b = (colorHex) & 0xFF;
    sdlColor.a = 255;

    if (r <= 0.5f)  // too small to draw a nice arc; just draw a point
    {
        glm::uvec4 col = { (colorHex >> 16) & 0xFF, (colorHex >> 8) & 0xFF, colorHex & 0xFF, 255 };
        glm::vec2 p = { c.x, c.y };
        game->renderer()->drawPoint(p, sdlColor, GameLayer::GRID_DISPLAY);
        return;
    }

    // --- Color ---
    glm::uvec4 col = {
        (colorHex >> 16) & 0xFF,
        (colorHex >> 8) & 0xFF,
        (colorHex) & 0xFF,
        255
    };

    // --- Segment count ---
    // Aim for ~2 px chord length. Clamp for sanity.
    const float circumference = 2.0f * B2_PI * r;
    int sides = static_cast<int>(circumference / 2.0f);
    if (sides < 12)  sides = 12;          // minimum smoothness
    if (sides > 96)  sides = 96;          // avoid huge loops in debug

    // --- Incremental rotation setup ---
    const float dA = (2.0f * B2_PI) / static_cast<float>(sides);
    const float cs = cosf(dA);
    const float sn = sinf(dA);

    // Start vector at angle 0 (r,0) and rotate around the center
    b2Vec2 v = { r, 0.0f };

    // First point
    glm::vec2 prev = { c.x + v.x, c.y + v.y };

    for (int i = 1; i <= sides; ++i)
    {
        // Rotate v by dA:  v' = R(dA) * v
        b2Vec2 vNext = { v.x * cs - v.y * sn, v.x * sn + v.y * cs };
        glm::vec2 curr = { c.x + vNext.x, c.y + vNext.y };

        // Draw edge
        game->renderer()->drawLine(prev, curr, col, GameLayer::GRID_DISPLAY);

        v = vNext;
        prev = curr;
    }

}

void Box2DDebugDrawAdapter::drawSolidCircle(const b2Transform& xf, float radius, b2HexColor colorHex)
{

    // Use center from transform and call the outline version
    const b2Vec2 center = xf.p;
    drawCircle(center, radius, colorHex);


    // 2) Now draw the rotation axis in the SAME space as drawCircle (screen)
    const float     S = GameConfig::instance().scaleFactor();
    const SDL_FRect cam = Camera::instance().frame();

    // Screen-space center and radius
    const float cx = center.x * S - cam.x;
    const float cy = center.y * S - cam.y;
    const float r = radius * S;

    // Axis (unit vector in local X) rotated by xf.q
    const b2Vec2 ax = b2Rot_GetXAxis(xf.q);

    // Endpoints in screen space
    glm::vec2 a0 = { cx, cy };
    glm::vec2 a1 = { cx + ax.x * r, cy + ax.y * r };

    // Color
    const glm::uvec4 col = {
        (colorHex >> 16) & 0xFF,
        (colorHex >> 8) & 0xFF,
        (colorHex) & 0xFF,
        255
    };

    game->renderer()->drawLine(a0, a1, col, GameLayer::GRID_DISPLAY);

}

void Box2DDebugDrawAdapter::drawSolidCapsule(const b2Vec2& p1, const b2Vec2& p2, float radius, b2HexColor c)
{
    // TODO: draw capsule (two semicircles + rectangle)
    (void)p1; (void)p2; (void)radius; (void)c;
}

void Box2DDebugDrawAdapter::drawSegment(const b2Vec2& p1, const b2Vec2& p2, b2HexColor c)
{
    // TODO: line p1->p2
    (void)p1; (void)p2; (void)c;
}

void Box2DDebugDrawAdapter::drawTransformXf(const b2Transform& xf)
{
    // TODO: draw local axes from xf.p using xf.q
    (void)xf;
}

void Box2DDebugDrawAdapter::drawPoint(const b2Vec2& p, float size, b2HexColor c)
{
    // TODO: small square/circle at p (size in pixels or meters, your choice)
    (void)p; (void)size; (void)c;
}

void Box2DDebugDrawAdapter::drawString(const b2Vec2& p, const char* s, b2HexColor c)
{
    // TODO: draw text at p (optional)
    (void)p; (void)s; (void)c;
}

DDColor Box2DDebugDrawAdapter::toColor(b2HexColor hex, float a)
{
    // b2HexColor is 0xRRGGBB
    float r = ((hex >> 16) & 0xFF) / 255.0f;
    float g = ((hex >> 8) & 0xFF) / 255.0f;
    float b = (hex & 0xFF) / 255.0f;
    return { r, g, b, a };
}

void Box2DDebugDrawAdapter::drawPolygon_old(const b2Vec2* vertices, int vertexCount, b2HexColor colorHex)
{

    if (vertexCount < 2 || vertices == nullptr)
        return;

    const float     S = GameConfig::instance().scaleFactor();
    const SDL_FRect cam = Camera::instance().frame();

    std::vector<SDL_FPoint> points;
    points.reserve(static_cast<size_t>(vertexCount) + 1);

    // Convert world (meters) -> screen (pixels) and apply camera offset
    for (int i = 0; i < vertexCount; ++i)
    {
        const b2Vec2& v = vertices[i];
        SDL_FPoint p;
        p.x = v.x * S - cam.x;
        p.y = v.y * S - cam.y;
        points.push_back(p);
    }

    // Close the polygon
    points.push_back(points.front());

    // Convert Box2D 0xRRGGBB to SDL color (opaque)
    SDL_Color c;
    c.r = (colorHex >> 16) & 0xFF;
    c.g = (colorHex >> 8) & 0xFF;
    c.b = (colorHex) & 0xFF;
    c.a = 255;

    // Your renderer draws connected line segments
    game->renderer()->drawPoints(points, c, GameLayer::GRID_DISPLAY);

}
