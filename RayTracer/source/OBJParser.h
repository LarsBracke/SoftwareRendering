#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "EMath.h"

class OBJParser
{
public:
	OBJParser();
	~OBJParser() = default;
	void Read(const std::string & path);
	std::vector<Elite::FPoint3>& GetVertices();
	std::vector<unsigned int>& GetIndices();
	void RemoveDuplicateVertices();
private:
	std::ifstream m_Input;
	//std::vector<std::string> m_Comments;
	std::vector<Elite::FPoint3> m_Vertices;
	std::vector<unsigned int> m_Indices;
};



