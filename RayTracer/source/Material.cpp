#include "Material.h"
#include <iostream>

Material::Material(float diffuseReflectance, Elite::RGBColor diffuseColor)
	: m_DiffuseReflectance{ diffuseReflectance }
	, m_DiffuseColor{ diffuseColor }
{ }

Material_Lambert::Material_Lambert(float diffuseReflectance, Elite::RGBColor diffuseColor)
	: Material(diffuseReflectance, diffuseColor)
{ }

Elite::RGBColor Material_Lambert::Shade(const HitRecord hitRecord, const Elite::FVector3 incomingDir, const Elite::FVector3 outgoingDir)
{
	return BRDF::Lambert(m_DiffuseReflectance, m_DiffuseColor);
}

Elite::RGBColor BRDF::Lambert(float diffuseReflectance, Elite::RGBColor diffuseColor)
{
	Elite::RGBColor reflectivity{ diffuseColor * diffuseReflectance };
	return reflectivity / float(E_PI);
}

Elite::RGBColor BRDF::Phong(float specularReflectance, float phongExponent, const Elite::FVector3 incomingDir, const Elite::FVector3 outgoingDir, Elite::FVector3 normal)
{
	Elite::FVector3 reflectVector{ incomingDir - 2 * (Elite::Dot(normal, incomingDir)) * normal };
	float cosAngle{ Elite::Dot(reflectVector, outgoingDir) };
	float specularReflection{specularReflectance * pow(cosAngle, phongExponent)};
	Elite::RGBColor reflectivity{ Elite::RGBColor{1,1,1} * specularReflection };
	return reflectivity;
}

Elite::RGBColor BRDF::CookTorrance(Elite::RGBColor albedoColor, float roughness, Elite::FVector3 incomingDir, Elite::FVector3 outgoingDir, Elite::FVector3 normal)
{
	float alpha{ (float)pow(roughness,2) };
	float alphaRemapped{ (float)pow((alpha + 1),2) / 8.f };	// direct lighting
	//float alphaRemapped{ pow(alpha,2) / 2 };			// indirect lighting

	Elite::FVector3 halfVector{ (incomingDir + outgoingDir) / Elite::Magnitude(incomingDir + outgoingDir) };
	Elite::Normalize(halfVector);

	// throwbridge-reitz ggx (normal distribution function)
	float d
	{
		(float)pow(alpha, 2) /
		(float)( float(E_PI) * pow(( pow(Elite::Dot(normal, halfVector), 2) * (pow(alpha, 2) - 1) + 1 ), 2) )
	};

	// schlick (fresnel funtion)
	Elite::RGBColor f
	{
		albedoColor + 
		(
		( Elite::RGBColor(1,1,1) - albedoColor ) *
		pow(( 1 - Elite::Dot(halfVector, outgoingDir)), 5)
		)
	};

	// schlick ggx (geometry function)
	float g
	{
		(Elite::Dot(outgoingDir, normal)) /
		( ( (Elite::Dot(outgoingDir, outgoingDir)) ) * ( 1 - alphaRemapped ) + alphaRemapped )
	};


	g *=
	float
	{
		Elite::Dot(normal, incomingDir) /
		( ( Elite::Dot(normal, incomingDir) ) * ( 1 - alphaRemapped ) + alphaRemapped )
	};


	// calculate final color
	Elite::RGBColor finalColor{ f  * g * d };
	float normalisation{ (4 * ((Elite::Dot(outgoingDir, normal)) * Elite::Dot(incomingDir, normal))) };
	finalColor /= normalisation;

	return finalColor;

}

Material_LambertPhong::Material_LambertPhong(float diffuseReflectance, Elite::RGBColor diffuseColor, float specularReflectance, float phongExponent)
	: Material(diffuseReflectance, diffuseColor)
	, m_SpecularReflectance{ specularReflectance }
	, m_PhongExponent{ phongExponent }
{ }

Elite::RGBColor Material_LambertPhong::Shade(const HitRecord hitRecord, const Elite::FVector3 incomingDir, const Elite::FVector3 outgoingDir)
{
	return
	BRDF::Lambert(m_DiffuseReflectance, m_DiffuseColor) +
		BRDF::Phong(m_SpecularReflectance, m_PhongExponent, incomingDir, outgoingDir, hitRecord.normal);
}

Material_PBR::Material_PBR(float diffuseReflectance, Elite::RGBColor diffuseColor, float specularReflectance, float roughness, Elite::RGBColor albedoColor, bool metalness)
	: Material(diffuseReflectance, diffuseColor)
	, m_SpecularReflectance{ specularReflectance }
	, m_Roughness{ roughness }
	, m_Metalness{ metalness }
{
	if (m_Metalness)
	{
		m_AlbedoColor = albedoColor;
	}
	else
	{
		m_AlbedoColor = Elite::RGBColor(0.4f, 0.4f, 0.4f);
	}
}

Elite::RGBColor Material_PBR::Shade(const HitRecord hitRecord, const Elite::FVector3 incomingDir, const Elite::FVector3 outgoingDir)
{
	Elite::RGBColor kd{  };
	Elite::FVector3 halfVector{ (incomingDir + outgoingDir) / Elite::Magnitude(incomingDir + outgoingDir) };
	Elite::Normalize(halfVector);
	Elite::RGBColor f{ m_AlbedoColor +
	(Elite::RGBColor{1 - m_AlbedoColor.r, 1 - m_AlbedoColor.g, 1 - m_AlbedoColor.b}) *
	(1 - (Elite::Dot(halfVector, outgoingDir))) };

	if (m_Metalness)
	{
		kd = Elite::RGBColor(0, 0, 0);
	}
	else
	{
		kd = Elite::RGBColor(1, 1, 1) - f;
	}

	Elite::RGBColor color =
	BRDF::CookTorrance(m_AlbedoColor, m_Roughness, incomingDir, outgoingDir, hitRecord.normal)
	+
	kd * BRDF::Lambert(m_DiffuseReflectance, m_DiffuseColor);
	
	return color;
}
