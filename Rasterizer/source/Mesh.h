#pragma once
#include "Object.h"
#include "Triangle.h"

class Mesh : public Object
{
public:
	Mesh(const std::vector<Vertex>& vertices, 
		const std::vector<unsigned int>& indices);
	virtual ~Mesh();
	virtual void Update(float deltaT);
	virtual bool Hit(const Elite::FPoint2& pixel, const std::vector<Vertex>& vertices, VertexRecord& hitRecord);
private:
	std::vector<Vertex> m_VertexBuffer;
	std::vector<unsigned int> m_IndexBuffer;
};

