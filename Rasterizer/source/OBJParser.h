#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "EMath.h"
#include "Vertex.h"
#include "Triangle.h"

class OBJParser
{
public:
	OBJParser();
	~OBJParser() = default;
	void Read(const std::string & path);
	std::vector<Vertex>& GetVertexBuffer();
	std::vector<unsigned int>& GetIndexBuffer();
	void RemoveDuplicateVertices();
private:
	std::ifstream m_Input;
	std::vector<Vertex> m_VertexBuffer;
	std::vector<unsigned int> m_IndexBuffer;

	//std::vector<std::string> m_Comments;
	std::vector<Elite::FPoint3> m_VertexPositions;
	std::vector<Elite::FVector3> m_VertexNormals;
	std::vector<Elite::FVector3> m_VertexUVs;
};



