#include "Quad.h"

Quad::Quad(const std::vector<Vertex>& vertices,
	std::vector<unsigned int>& listIndices,
	std::vector<unsigned int>& stripIndices,
	PrimitiveTopology primitiveTopology)
	: m_ListIndexBuffer{ listIndices }
	, m_StripIndexBuffer{ listIndices }
	, m_PrimitiveTopology{ primitiveTopology }
{
	m_Vertices = vertices;
}

bool Quad::Hit(const Elite::FPoint2& pixel, const std::vector<Vertex>& vertices, VertexRecord& hitRecord)
{
	switch (m_PrimitiveTopology)
	{
	case PrimitiveTopology::TriangleList:

		for (unsigned int currentindex = 0; (uint64_t(currentindex) + 2) < m_ListIndexBuffer.size(); currentindex += 3)
		{
			unsigned int index0 = m_ListIndexBuffer[uint64_t(currentindex)];
			unsigned int index1 = m_ListIndexBuffer[uint64_t(currentindex) + 1];
			unsigned int index2 = m_ListIndexBuffer[uint64_t(currentindex) + 2];

			std::vector<Vertex> triangleVertices{ vertices[index0], vertices[index1], vertices[index2] };
			Triangle t{ triangleVertices };
			if (t.Hit(pixel, triangleVertices, hitRecord))
			{
				m_Weights = t.GetWeights();
				return true;
			}
		}

		break;
	case PrimitiveTopology::TriangleStrip:

		for (unsigned int currentIndex = 0; currentIndex < m_StripIndexBuffer.size(); ++currentIndex)	
		{
			unsigned int index0 = m_StripIndexBuffer[uint64_t(currentIndex)];
			unsigned int index1 = m_StripIndexBuffer[uint64_t(currentIndex) + 1];
			unsigned int index2 = m_StripIndexBuffer[uint64_t(currentIndex) + 2];

			if (index0 == index1 || index1 == index2 || index2 == index0) continue;

			std::vector<Vertex> triangleVertices;
			if (currentIndex % 2 == 0)
			{
				triangleVertices = { vertices[index0], vertices[index1], vertices[index2] };
			}
			else
			{
				triangleVertices = { vertices[index0], vertices[index2], vertices[index1] };
			}

			Triangle t{ triangleVertices };
			if (t.Hit(pixel, vertices, hitRecord))
			{
				return true;
			}
		}

		break;
	}

	return false;
}

void Quad::Update(float deltaT)
{ }
