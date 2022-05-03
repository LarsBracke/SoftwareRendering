#pragma once
#include "Object.h"
#include "Triangle.h"

enum class PrimitiveTopology
{
	TriangleList,
	TriangleStrip
};

class Quad : public Object
{
public:
	Quad(const std::vector<Vertex>& vertices, 
		std::vector<unsigned int>& listIndices,
		std::vector<unsigned int>& stripIndices,
		PrimitiveTopology primitiveTopology = PrimitiveTopology::TriangleList);
	virtual ~Quad() override = default;
	virtual bool Hit(const Elite::FPoint2& pixel, const std::vector<Vertex>& vertices, VertexRecord& hitRecord);
	virtual void Update(float deltaT);
private:
	std::vector<unsigned int> m_ListIndexBuffer;
	std::vector<unsigned int> m_StripIndexBuffer;

	PrimitiveTopology m_PrimitiveTopology;
};

