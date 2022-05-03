#pragma once
#include "Light.h"

class LightManager final
{
public:
	~LightManager();
	static LightManager& GetInstance();
	void AddLight(Light* pLight);
	const std::vector<Light*>& GetLights() const;
private:
	LightManager();
	std::vector<Light*> m_pLights;
};

