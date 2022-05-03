#pragma once
#include "EMath.h"

struct Ray
{
	Elite::FPoint3 origin;
	Elite::FVector3 direction;
	float tMin;
	float tMax;
};

struct HitRecord
{
	Elite::FPoint3 hitPoint;
	Elite::FVector3 normal;
	float tValue;
};