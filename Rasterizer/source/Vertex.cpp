#include "Vertex.h"

Vertex::Vertex()
	: position{ }
	, color{ }
	, normal{ }
	, tangent{ }
	, uv{ }
{ }

Vertex::Vertex(const Elite::FPoint4& position, const Elite::RGBColor& color)
	: position{ position }
	, uv{ }
	, color{ color }
	, normal{ }
	, tangent{ }
{ }

Vertex::Vertex(const Elite::FPoint4& position, const Elite::FVector2& uv, const Elite::RGBColor& color)
	: position{ position }
	, uv{ uv }
	, color{ color }
	, normal{ }
	, tangent{ }
{ }


