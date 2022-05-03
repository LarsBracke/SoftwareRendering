#pragma once
#include "EMath.h"
#include "ERGBColor.h"

struct Vertex final
{
	Vertex();
	Vertex(const Elite::FPoint4& position, 
		const Elite::RGBColor& color = { 1,1,1 });

	Vertex(const Elite::FPoint4& position, const Elite::FVector2& uv,
		const Elite::RGBColor& color = { 1,1,1 });

	Elite::FPoint4 position;
	Elite::FVector2 uv;
	Elite::FVector3 normal;
	Elite::FVector3 tangent;
	Elite::RGBColor color;

};

