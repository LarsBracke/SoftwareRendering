#pragma once
#include "Object.h"
#include "Triangle.h"

#include <vector>
#include <iostream>

class SceneGraph final
{
public:
	~SceneGraph();
	static SceneGraph& GetInstance();
	void UpdateObjects(float deltaT);
	void AddObjectToGraph(Object* object);
	std::vector<Object*>& GetObjects();
private:
	SceneGraph();
	std::vector<Object*> m_pObjects;
};

