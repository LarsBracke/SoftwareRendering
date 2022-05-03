#pragma once
#include "Ray.h"
#include "EMathUtilities.h"
#include "ERGBColor.h"

class Material
{
public:
	Material(float diffuseReflectance, Elite::RGBColor diffuseColor);
	virtual ~Material() = default;
	virtual Elite::RGBColor Shade(const HitRecord hitRecord, const Elite::FVector3 incomingDir, const Elite::FVector3 outgoingDir) = 0;
protected:
	float m_DiffuseReflectance;
	Elite::RGBColor m_DiffuseColor;
};

namespace BRDF
{
	Elite::RGBColor Lambert(float diffuseReflectance, Elite::RGBColor diffuseColor);
	Elite::RGBColor Phong
	(float specularReflectance, float phongExponent, 
		const Elite::FVector3 incomingDir, const Elite::FVector3 outgoingDir, 
		Elite::FVector3 normal);
	Elite::RGBColor CookTorrance(Elite::RGBColor albedoColor, float roughness,
		Elite::FVector3 incomingDir, Elite::FVector3 outgoingDir, Elite::FVector3 normal);
}

class Material_Lambert final : public Material
{
public:
	Material_Lambert(float diffuseReflectance, Elite::RGBColor diffuseColor);
	virtual ~Material_Lambert() override = default;
	virtual Elite::RGBColor Shade(const HitRecord hitRecord, const Elite::FVector3 incomingDir, const Elite::FVector3 outgoingDir);
};

class Material_LambertPhong final : public Material
{
public:
	Material_LambertPhong(float diffuseReflectance, Elite::RGBColor diffuseColor, float specularReflectance, float phongExponent);
	virtual ~Material_LambertPhong() override = default;
	virtual Elite::RGBColor Shade(const HitRecord hitRecord, const Elite::FVector3 incomingDir, const Elite::FVector3 outgoingDir);
private:
	float m_SpecularReflectance;
	float m_PhongExponent;

};

class Material_PBR : public Material
{
public:
	Material_PBR(float diffuseReflectance, Elite::RGBColor diffuseColor,
		float specularReflection, float roughness, 
		Elite::RGBColor albedoColor = Elite::RGBColor{ 0.4f,0.4f,0.4f }, bool metalness = false);
	virtual ~Material_PBR() override = default;
	virtual Elite::RGBColor Shade(const HitRecord hitRecord, const Elite::FVector3 incomingDir, const Elite::FVector3 outgoingDir);
private:
	float m_SpecularReflectance;
	float m_Roughness;
	Elite::RGBColor m_AlbedoColor;
	bool m_Metalness;
};