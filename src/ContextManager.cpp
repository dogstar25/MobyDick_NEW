#include "ContextManager.h"
#include "SoundManager.h"
#include "BaseConstants.h"
#include <assert.h>
#include <string>
#include "Util.h"



ContextManager::ContextManager()
{
}

void StatusItem::adjust(float adjustValue)
{
    if (m_value < m_maxValue || m_value > 0) {
        m_value += adjustValue;
    }
}

void ContextManager::clearStatusItems()
{
    m_statusValueMap.clear();


}

void ContextManager::addStatusItem(int id, StatusItem& statusItem)
{

    //Add it if tis not already there
    //There may be status items that shoudl retain their values across multiple levels and such
    if (m_statusValueMap.find(id) == m_statusValueMap.end()) {
        m_statusValueMap[id] = statusItem;
    }
}

float ContextManager::getStatusItemValue(int valueId)
{

    assert(m_statusValueMap.find(valueId) != m_statusValueMap.end() && "ValueId Name wasnt found in StatusValueMap");

    float value = m_statusValueMap[valueId].value();

    return value;

}

void ContextManager::adjustStatusItemValue(int valueId, float adjustmentValue)
{
    assert(m_statusValueMap.find(valueId) != m_statusValueMap.end() && "ValueId Name wasnt found in StatusValueMap");

    m_statusValueMap[valueId].adjust(adjustmentValue);

}

void ContextManager::setStatusItemValue(int valueId, float newValue)
{
    assert(m_statusValueMap.find(valueId) != m_statusValueMap.end() && "ValueId Name wasnt found in StatusValueMap");

    m_statusValueMap[valueId].setValue(newValue);

}

void ContextManager::setMouseSensitivity(int mouseSensitivity)
{

    m_userSettings.mouseSensitivity = mouseSensitivity;

}

void ContextManager::setSoundVolume(int volume)
{

    m_userSettings.soundLevel = volume;

}

int ContextManager::getMouseSensitivityForGui()
{
    return  m_userSettings.mouseSensitivity;

}

int ContextManager::getSoundVolumeForGui()
{
	return  m_userSettings.soundLevel;

}

float ContextManager::getMouseSensitivity()
{
	return  m_userSettings.mouseSensitivity;

}
int ContextManager::getSoundVolume()
{
	return  (int)(m_userSettings.soundLevel);

}

StatusItem& ContextManager::getStatusItem(int valueId)
{

    //return an empty
    if (m_statusValueMap.find(valueId) == m_statusValueMap.end()) {
        return  m_statusValueMap[StatusItemId::DEFAULT_EMPTY];

    }

    return m_statusValueMap[valueId];


}

