#include "Object.h"

Object::Object(const Elite::FPoint3& origin, const Elite::RGBColor& color)
	: m_Pivot{ origin }
	, m_Color{ color }
	, m_Rotation{ Elite::FMatrix3::Identity() }
	, m_Translation{ Elite::FVector3::ZeroVector() }
{ }

Elite::RGBColor Object::GetColor() const
{
	return m_Color;
}

void Object::SetTranslation(const Elite::FVector3 translation)
{
	m_Translation = translation;
}

const std::vector<Vertex>& Object::GetVertices() const
{
	return m_Vertices;
}

const std::vector<float>& Object::GetWeights() const
{
	return m_Weights;
}