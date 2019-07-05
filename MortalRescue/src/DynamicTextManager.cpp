#include "DynamicTextManager.h"



DynamicTextManager::DynamicTextManager()
{

	textItem* item = new textItem();
	item->text = "defaultText";
	item->hasChanged = true;
	this->textItems["DEFAULT"] = make_unique<textItem>(*item);

}


DynamicTextManager::~DynamicTextManager()
{

	this->textItems.clear();
}

textItem* DynamicTextManager::getTextItem(string id)
{
	textItem* textItem;

	auto iter = this->textItems.find(id);

	if (iter != this->textItems.end())
	{

		textItem = this->textItems[id].get();

	}
	else
	{
		textItem = this->textItems["DEFAULT"].get();
	}

	return textItem;

}


void DynamicTextManager::updateText(string id, string newText)
{

	//If the text item is already in the map, then update it, otherwise create it
	auto iter = this->textItems.find(id);
	
	if (iter != this->textItems.end())
	{
		if(iter->second->text.compare(newText) != 0)
		{
			iter->second->text = newText;
			iter->second->hasChanged = true;
		}
		else
		{
			iter->second->hasChanged = false;
		}

	}
	else
	{
		textItem* item = new textItem();
		item->text = newText;
		item->hasChanged = true;
		this->textItems[id] = make_unique<textItem>(*item);
	}

}
