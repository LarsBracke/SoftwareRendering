#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include "Vertex.h"

#include <vector>


struct VertexRecord
{
	float depthInterpolated;
	Elite::FVector3 colorInterpolated;
	Elite::FVector2 uvInterpolated;
	Elite::FVector3 normalInterpolated;
	Elite::FVector3 tangentInterpolated;
};

class Object
{
public:
	Object(const Elite::FPoint3& origin = { 0,0,0 }, const Elite::RGBColor& color = { 0.f, 0.f, 0.f });
	virtual ~Object() = default;
	virtual void Update(float deltaT) = 0;
	virtual bool Hit(const Elite::FPoint2& pixel, const std::vector<Vertex>& vertices, VertexRecord& hitRecord) = 0;
	Elite::RGBColor GetColor() const;

	void SetTranslation(const Elite::FVector3 translation);

	const std::vector<Vertex>& GetVertices() const;
	const std::vector<float>& GetWeights() const;
protected:
	Elite::FPoint3 m_Pivot; 
	Elite::RGBColor m_Color;

	std::vector<Vertex> m_Vertices;
	std::vector<float> m_Weights;

	Elite::FVector3 m_Translation;
	Elite::FMatrix3 m_Rotation;
};