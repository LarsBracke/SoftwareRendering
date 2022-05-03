#pragma once
#include "Object.h"
#include "Vertex.h"
#include <vector>
#include <assert.h>

class Triangle final : public Object
{
public:
	Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2,
		const Elite::RGBColor& color = {1,1,1});
	Triangle(const std::vector<Vertex>& vertices,
		const Elite::RGBColor& color = { 1,1,1 });
	~Triangle() override = default;
	virtual void Update(float deltaT);
	virtual bool Hit(const Elite::FPoint2& pixel, const std::vector<Vertex>& vertices, VertexRecord& hitRecord);

};

