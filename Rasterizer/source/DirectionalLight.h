#pragma once
#include "EMath.h"
#include "ERGBColor.h"

class DirectionalLight
{
public:
	DirectionalLight(Elite::RGBColor lightColor, float lightIntensity, Elite::FVector3 direction);
	~DirectionalLight() = default;
	Elite::RGBColor GetColor() const;
	Elite::FVector3 GetDirection() const ;
	float GetIntensity() const;
private:
	Elite::RGBColor m_Color;
	float m_Intensity;
	Elite::FVector3 m_Direction;
};

