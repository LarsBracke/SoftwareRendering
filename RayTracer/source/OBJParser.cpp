#include "OBJParser.h"
#include <iostream>

OBJParser::OBJParser()
{ }

void OBJParser::Read(const std::string& path)
{
	std::cout << "parsing..." << std::endl;

	std::string line;
	m_Input.open(path, std::ios::in);
	if (m_Input.is_open())
	{
		while (!m_Input.eof())
		{
			std::getline(m_Input, line);

			//// comments
			//if (line[0] == '#')
			//{
			//	line.erase(line.begin(), line.begin() + 2);
			//	m_Comments.push_back(line);
			//	line.clear();
			//}

			// vertices
			if (line[0] == 'v')
			{
				line.erase(line.begin(), line.begin() + 3);
				Elite::FPoint3 vertex;

				// x
				std::string::iterator spacing{ std::find(line.begin(), line.end(), ' ') };
				vertex.x = std::stof(std::string{ line.begin(), spacing });
				line.erase(line.begin(), spacing + 1);

				// y
				spacing = std::find(line.begin(), line.end(), ' ');
				vertex.y = std::stof(std::string{ line.begin(), spacing });
				line.erase(line.begin(), spacing + 1);

				// z
				vertex.z = std::stof(std::string{ line.begin(), line.end() });

				line.clear();
				m_Vertices.push_back(vertex);
			}

			// faces
			else if (line[0] == 'f')
			{
				line.erase(line.begin(), line.begin() + 2);
				int index;

				// first index
				std::string::iterator spacing{ std::find(line.begin(), line.end(), ' ') };
				index = std::stoi(std::string(line.begin(), spacing));
				m_Indices.push_back(index - 1);
				line.erase(line.begin(), spacing + 1);

				// second index
				spacing = std::find(line.begin(), line.end(), ' ');
				index = std::stoi(std::string{ line.begin(), spacing });
				m_Indices.push_back(index - 1);
				line.erase(line.begin(), spacing + 1);

				// third index
				index = std::stoi(std::string(line.begin(), line.end()));
				m_Indices.push_back(index - 1);

				line.clear();
			}
		}
	}
	std::cout << "parsing finished" << std::endl;
}

std::vector<Elite::FPoint3>& OBJParser::GetVertices()
{
	return m_Vertices;
}

std::vector<unsigned int>& OBJParser::GetIndices()
{
	return m_Indices;
}

void OBJParser::RemoveDuplicateVertices()
{
	for (std::vector<Elite::FPoint3>::iterator it1 = m_Vertices.begin(); it1 != m_Vertices.end(); ++it1)
	{
		for (std::vector<Elite::FPoint3>::iterator it2 = m_Vertices.begin(); it2 != m_Vertices.end(); ++it2)
		{
			if (it1 != it2 && (*it1) == (*it2))
			{
				m_Vertices.erase(it1);
			}
		}
	}
}
