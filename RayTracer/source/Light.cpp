#include "Light.h"
#include <iostream>

Light::Light(Elite::FPoint3 position, Elite::RGBColor lightColor, float lightIntensity)
	: m_Position{ position }
	, m_LightColor{ lightColor }
	, m_LightIntensity{ lightIntensity }
{ }

Elite::FPoint3 Light::GetPosition() const
{
	return m_Position;
}

PointLight::PointLight(Elite::FPoint3 position, Elite::RGBColor lightColor, float lightIntensity)
	: Light(position, lightColor, lightIntensity)
{ }

Elite::FVector3 PointLight::GetDirection(Elite::FPoint3 pointToShade) const
{
	return (m_Position - pointToShade);
}

Elite::RGBColor PointLight::CalculateColor(Elite::FPoint3 point)
{
	Elite::RGBColor color =
		(m_LightColor) * (m_LightIntensity / Elite::SqrMagnitude(m_Position - point));
	return color;
}

DirectionalLight::DirectionalLight(Elite::FPoint3 position, Elite::RGBColor lightColor, float lightIntensity, Elite::FVector3 direction)
	: Light(position, lightColor, lightIntensity)
	, m_Direction{ direction }
{ }

Elite::FVector3 DirectionalLight::GetDirection(Elite::FPoint3 pointToShade) const
{
	return m_Direction;
}

Elite::RGBColor DirectionalLight::CalculateColor(Elite::FPoint3 point)
{
	Elite::RGBColor color
	{ m_LightColor * m_LightIntensity };
	return color;
}
