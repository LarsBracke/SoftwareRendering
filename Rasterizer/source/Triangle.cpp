#include "Triangle.h"
#include <iostream>

Triangle::Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2,
	const Elite::RGBColor& color)
{
	m_Vertices.push_back(v0);
	m_Vertices.push_back(v1);
	m_Vertices.push_back(v2);
	m_Color = color;
	m_Weights = std::vector<float>(3);
}

Triangle::Triangle(const std::vector<Vertex>& vertices, const Elite::RGBColor& color)
{
	m_Vertices = vertices;
	m_Color = color;
	m_Weights = std::vector<float>(3);
}

void Triangle::Update(float deltaT)
{
	return;
}

bool Triangle::Hit(const Elite::FPoint2& pixel, const std::vector<Vertex>& vertices, VertexRecord& vertexRecord)
{
	// calculate sides
	Elite::FVector3 side0{ vertices[1].position - vertices[0].position };
	Elite::FVector3 side1{ vertices[2].position - vertices[0].position };
	Elite::Normalize(side0);
	Elite::Normalize(side1);

	// calculate normal
	Elite::FVector3 normal{ Elite::Cross(side0, side1) };
	Elite::Normalize(normal);
	
	// check if the pixel is in the triangle
	Elite::FVector2 edgeA{ vertices[1].position - vertices[0].position };
	Elite::FVector2 pointToSide{ pixel - Elite::FPoint2(vertices[0].position) };
	Elite::Normalize(pointToSide);
	float cross0{ Elite::Cross(edgeA, pointToSide) };
	if (cross0 > 0.f) return false;

	Elite::FVector2 edgeB{ vertices[2].position - vertices[1].position };
	pointToSide = Elite::FVector2{ pixel - Elite::FPoint2(vertices[1].position) };
	Elite::Normalize(pointToSide);
	float cross1{ Elite::Cross(edgeB, pointToSide) };
	if (cross1 > 0.f) return false;

	Elite::FVector2 edgeC{ vertices[2].position - vertices[0].position };
	pointToSide = Elite::FVector2{pixel - Elite::FPoint2(vertices[2].position) };
	Elite::Normalize(pointToSide);
	float cross2{ Elite::Cross(edgeC, pointToSide) };
	if (cross2 < 0.f) return false;

	float totalArea =
		Elite::Cross(edgeA, edgeC);
	
	m_Weights[0] = Elite::Cross((pixel - vertices[1].position.xy), (vertices[2].position - vertices[1].position).xy) / -totalArea;
	m_Weights[1] = Elite::Cross((pixel - vertices[2].position.xy), (vertices[0].position - vertices[2].position).xy) / -totalArea;
	m_Weights[2] = Elite::Cross((pixel - vertices[0].position.xy), (vertices[1].position - vertices[0].position).xy) / -totalArea;

	vertexRecord.uvInterpolated =
		(
			(vertices[0].uv / vertices[0].position.z * m_Weights[0]) +
			(vertices[1].uv / vertices[1].position.z * m_Weights[1]) + 
			(vertices[2].uv / vertices[2].position.z * m_Weights[2])
		);

	vertexRecord.normalInterpolated =
		(
			(vertices[0].normal / vertices[0].position.z * m_Weights[0]) +
			(vertices[1].normal / vertices[1].position.z * m_Weights[1]) +
			(vertices[2].normal / vertices[2].position.z * m_Weights[2])
		);
	Elite::Normalize(vertexRecord.normalInterpolated);

	//vertexRecord.tangentInterpolated =
	//	(
	//		(vertices[0].tangent / vertices[0].position.z * m_Weights[0]) +
	//		(vertices[1].tangent / vertices[1].position.z * m_Weights[1]) +
	//		(vertices[2].tangent / vertices[2].position.z * m_Weights[2])
	//		);
	//Elite::Normalize(vertexRecord.tangentInterpolated);

	vertexRecord.depthInterpolated =
		1.f /
		(
			((1.f / vertices[0].position.z) * m_Weights[0]) + 
			((1.f / vertices[1].position.z) * m_Weights[1]) + 
			((1.f / vertices[2].position.z) * m_Weights[2]) 
		);

	return true;
}
