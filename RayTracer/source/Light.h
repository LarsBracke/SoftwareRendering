#pragma once
#include <vector>
#include "EMath.h"
#include "ERGBColor.h"

class Light
{
public:
	Light(Elite::FPoint3 position, Elite::RGBColor lightColor, float lightIntensity);
	virtual ~Light() = default;
	Elite::FPoint3 GetPosition() const;
	virtual Elite::FVector3 GetDirection(Elite::FPoint3 pointToShade) const = 0;
	virtual Elite::RGBColor CalculateColor(Elite::FPoint3 point) = 0;
protected:
	Elite::FPoint3 m_Position;
	Elite::RGBColor m_LightColor;
	float m_LightIntensity;

};

class PointLight final : public Light
{
public:
	PointLight(Elite::FPoint3 position, Elite::RGBColor lightColor, float lightIntensity);
	~PointLight() override = default;
	Elite::FVector3 GetDirection(Elite::FPoint3 pointToShade) const override;
	Elite::RGBColor CalculateColor(Elite::FPoint3 point) override;
};

class DirectionalLight final : public Light
{
public:
	DirectionalLight(Elite::FPoint3 position, Elite::RGBColor lightColor, float lightIntensity, Elite::FVector3 direction);
	virtual ~DirectionalLight() = default;
	virtual Elite::FVector3 GetDirection(Elite::FPoint3 pointToShade) const;
	virtual Elite::RGBColor CalculateColor(Elite::FPoint3 point);
private:
	Elite::FVector3 m_Direction;
};

