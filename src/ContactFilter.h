#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <bitset>

#include <box2d/box2d.h>

#include "Util.h"
#include "BaseConstants.h"

class ContactFilter
{
public:
	ContactFilter();
	static ContactFilter& instance();

	static bool ShouldCollide(b2ShapeId shapeAId, b2ShapeId shapeBId, void* context );

protected:
	static std::vector<std::bitset<ContactTag::MAX_OBJECT_CATEGORIES>> m_contactMasks;

};

