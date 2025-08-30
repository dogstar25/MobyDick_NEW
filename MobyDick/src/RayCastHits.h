#pragma once
#include <vector>
#include "box2d/box2d.h"

struct RayResultItem
{
    b2ShapeId shapeId;
    b2Vec2 point;
    b2Vec2 normal;
    float fraction;
};

struct RayContext
{
    std::vector<RayResultItem> hits;
};


std::vector<RayResultItem> RayCastHits(b2WorldId world,
    const b2Vec2& start,
    const b2Vec2& end);

struct intersection_sort_compare
{
    inline bool operator() (const RayResultItem& item1, const RayResultItem& item2)
    {
        return (item1.fraction < item2.fraction);
    }
};

