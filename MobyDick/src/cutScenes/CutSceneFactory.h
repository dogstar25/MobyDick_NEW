#pragma once
#include <memory>

#include "CutScene.h"

class CutSceneFactory
{
public:
	
	virtual std::shared_ptr<CutScene> create(std::string cutSceneType);

private:

};
