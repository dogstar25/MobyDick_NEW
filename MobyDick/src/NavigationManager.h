#pragma once

#include <vector>
#include <optional>


#include "GameObject.h"


struct NavigationMapItem
{
	bool passable{ true };
	std::optional<std::weak_ptr<GameObject>>gameObject{};
};




class NavigationManager
{

public:

	NavigationManager() = default;

	virtual void buildNavigationMapItem(GameObject* gameObject, Scene* scene);
	virtual void updateNavigationMap();

	
	std::vector < std::vector<NavigationMapItem>>& navigationMap() { return m_navigationMap; }
	void setNavigationMapArraySize(int width, int height);
	bool navigationMapChanged() { return m_navigationMapChanged; }


protected:

	bool m_navigationMapChanged{};
	std::vector < std::vector<NavigationMapItem>> m_navigationMap{};



};

