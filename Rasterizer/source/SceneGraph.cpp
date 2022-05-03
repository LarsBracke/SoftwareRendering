#include "SceneGraph.h"

SceneGraph::~SceneGraph()
{
	std::cout << "SceneGraph destructor" << std::endl;
	for (Object* pObject : m_pObjects)
	{
		delete pObject;
		pObject = nullptr;
	}
}

SceneGraph& SceneGraph::GetInstance()
{
	static SceneGraph sceneGraph;
	return sceneGraph;
}

void SceneGraph::UpdateObjects(float deltaT)
{
	for (Object* pObject : m_pObjects)
	{
		pObject->Update(deltaT);
	}
}

void SceneGraph::AddObjectToGraph(Object* object)
{
	m_pObjects.push_back(object);
}

std::vector<Object*>& SceneGraph::GetObjects()
{
	return m_pObjects;
}

SceneGraph::SceneGraph()
{ }