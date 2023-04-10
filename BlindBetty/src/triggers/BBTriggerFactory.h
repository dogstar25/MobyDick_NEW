#pragma once
#include "MobyDick.h"

class BBTriggerFactory : public TriggerFactory
{
public:
	BBTriggerFactory();

	std::shared_ptr<Trigger> create(std::string triggerType) override;

private:

};

