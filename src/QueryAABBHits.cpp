#include "QueryAABBHits.h"
#include "GameObject.h"

struct QueryAABBContext 
{ 
    std::vector<b2ShapeId> hits; 
};

static bool QueryAABBCollectCallback(b2ShapeId shapeId, void* context)
{
    auto* ctx = static_cast<QueryAABBContext*>(context);

    //Ignore fragments
    b2BodyId bodyId = b2Shape_GetBody(shapeId);
    b2Body_GetUserData(bodyId);
    GameObject* gameObject = static_cast<GameObject*>(b2Body_GetUserData(bodyId));

    if (gameObject->hasTrait(TraitTag::fragment) == false) {

        ctx->hits.push_back(shapeId);
    }
    
    return 1.0f;
}

std::vector<b2ShapeId> QueryAABBHits(b2WorldId world, const b2AABB& aabb)
{
    QueryAABBContext ctx;
    b2World_OverlapAABB(
        world, aabb,
        b2DefaultQueryFilter(),
        QueryAABBCollectCallback,
        &ctx
    );
    return ctx.hits;
}

