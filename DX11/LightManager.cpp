#include "pch.h"
#include "Light.h"
#include "LightManager.h"

SINGLE_BODY(LightManager)

void LightManager::Init()
{
	
}

void LightManager::DeleteLight(InstanceID id)
{
	for (int i = 0; i < m_lightList.size(); i++)
	{
		if (m_lightList[i]->GetInstanceID() == id)
			m_lightList.erase(m_lightList.begin() + i);
	}
}
