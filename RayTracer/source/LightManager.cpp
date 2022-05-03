#include "LightManager.h"
#include <iostream>

LightManager::~LightManager()
{
	std::cout << "LightManager destructor" << std::endl;
	for (Light* pLight : m_pLights)
	{
		delete pLight;
		pLight = nullptr;
	}
}

LightManager& LightManager::GetInstance()
{
	static LightManager lightManager;
	return lightManager;
}

void LightManager::AddLight(Light* pLight)
{
	m_pLights.push_back(pLight);
}

const std::vector<Light*>& LightManager::GetLights() const
{
	return m_pLights;
}

LightManager::LightManager()
{ }
