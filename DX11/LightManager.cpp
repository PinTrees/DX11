#include "pch.h"
#include "LightManager.h"

SINGLE_BODY(LightManager)

LightManager::LightManager()
{

}

LightManager::~LightManager()
{

}

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

vector<DirectionalLight>& LightManager::GetDirLights()
{
	m_dirLights.clear();

	for (int i = 0; i < m_lightList.size(); i++)
	{
		if (m_lightList[i]->GetLightType() == LightType::Directional)
		{
			m_dirLights.push_back(m_lightList[i]->GetDirLight());
		}
	}

	return m_dirLights;
}

// true -> calculate, Not Input or false -> calculate X
vector<PointLight>& LightManager::GetPointLights(bool calc)
{
	if (calc)
	{
		m_pointLights.clear();

		for (int i = 0; i < m_lightList.size(); i++)
		{
			if (m_lightList[i]->GetLightType() == LightType::Point)
			{
				m_pointLights.push_back(m_lightList[i]->GetPointLight());
			}
		}
	}

	return m_pointLights;
}

// true -> calculate, Not Input or false -> calculate X
vector<SpotLight>& LightManager::GetSpotLights(bool calc)
{
	if (calc)
	{
		m_spotLights.clear();

		for (int i = 0; i < m_lightList.size(); i++)
		{
			if (m_lightList[i]->GetLightType() == LightType::Spot)
			{
				m_spotLights.push_back(m_lightList[i]->GetSpotLight());
			}
		}
	}

	return m_spotLights;
}
