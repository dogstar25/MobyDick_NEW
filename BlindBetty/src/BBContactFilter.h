#pragma once
#include "MobyDick.h"

class BBContactFilter : public ContactFilter
{
public:
	BBContactFilter();
	bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) final;

};

