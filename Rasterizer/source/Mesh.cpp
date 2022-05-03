#include "Mesh.h"
#include <iostream>
#include "SceneGraph.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
	: m_VertexBuffer{ vertices }
	, m_IndexBuffer{ indices }
{
	m_Vertices = m_VertexBuffer;

	for (unsigned int i = 0; i < m_IndexBuffer.size(); ++i)
	{
		m_IndexBuffer[i] = i;
	}

	for (unsigned int currentindex = 0; (currentindex + 2) < m_IndexBuffer.size(); currentindex += 3)
	{
		unsigned int index0 = m_IndexBuffer[currentindex];
		unsigned int index1 = m_IndexBuffer[currentindex + 1];
		unsigned int index2 = m_IndexBuffer[currentindex + 2];

		std::vector<Vertex> triangleVertices{ m_Vertices[index0], m_Vertices[index1], m_Vertices[index2] };
		Triangle* pT = new Triangle{ triangleVertices };
		SceneGraph::GetInstance().AddObjectToGraph(pT);
	}
}

Mesh::~Mesh()
{
}

void Mesh::Update(float deltaT)
{ }

bool Mesh::Hit(const Elite::FPoint2 & pixel, const std::vector<Vertex>& vertices, VertexRecord& hitRecord)
{
	//for (unsigned int i = 0; i < m_IndexBuffer.size(); ++i)
	//{
	//	m_IndexBuffer[i] = i;
	//}

	//for (unsigned int currentindex = 0; (currentindex + 2) < m_IndexBuffer.size(); currentindex += 3)
	//{
	//	unsigned int index0 = m_IndexBuffer[currentindex];
	//	unsigned int index1 = m_IndexBuffer[currentindex + 1];
	//	unsigned int index2 = m_IndexBuffer[currentindex + 2];

	//	std::vector<Vertex> triangleVertices{ vertices[index0], vertices[index1], vertices[index2] };
	//	Triangle t{ triangleVertices };
	//	if (t.Hit(pixel, triangleVertices, hitRecord))
	//	{
	//		m_Weights = t.GetWeights();
	//		//m_Vertices = vertices;
	//		return true;
	//	}
	//}

	//for (size_t i = 0; i+3 < vertices.size(); i+=3)
	//{
	//	std::vector<Vertex> tv{ vertices[i], vertices[i + 1], vertices[i + 2] };
	//	Triangle t{ tv };
	//	if (t.Hit(pixel, tv, hitRecord));
	//	{
	//		//m_Weights = t.GetWeights();
	//		return true;
	//	}
	//}
	return false;

}
