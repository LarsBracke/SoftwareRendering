#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Elite::RGBColor lightColor, float lightIntensity, Elite::FVector3 lightDirection)
    : m_Color{ lightColor }
    , m_Intensity{ lightIntensity }
    , m_Direction{ lightDirection }
{ }

Elite::RGBColor DirectionalLight::GetColor() const
{
    return m_Color;
}

Elite::FVector3 DirectionalLight::GetDirection() const
{
    return m_Direction;
}

float DirectionalLight::GetIntensity() const
{
    return m_Intensity;
}
