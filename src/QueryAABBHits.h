#pragma once
#include <vector>
#include "box2d/box2d.h"

std::vector<b2ShapeId> QueryAABBHits(b2WorldId world, const b2AABB& aabb);

