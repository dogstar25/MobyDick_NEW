#pragma once
#include "MobyDick.h"

class BBCutSceneFactory : public CutSceneFactory
{
public:
	
	std::shared_ptr<CutScene> create(std::string cutSceneType);

private:

};
