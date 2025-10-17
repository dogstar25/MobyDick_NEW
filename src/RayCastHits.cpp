#include "RayCastHits.h"

static float RayCollectCallback(b2ShapeId shapeId,
    b2Vec2 point, b2Vec2 normal, float fraction, void* context)
{

    auto* ctx = static_cast<RayContext*>(context);

    RayResultItem rayResultItem {
        shapeId,
        point,
        normal,
        fraction
    };

    ctx->hits.push_back(rayResultItem);
    return 1.0f;
}

std::vector<RayResultItem> RayCastHits(b2WorldId world,
    const b2Vec2& start,
    const b2Vec2& end)
{
    RayContext ctx;
    b2Vec2 translation = { end.x - start.x, end.y - start.y };
    b2World_CastRay(
        world, start, translation,
        b2DefaultQueryFilter(),
        RayCollectCallback,
        &ctx
    );

    return ctx.hits;
}

