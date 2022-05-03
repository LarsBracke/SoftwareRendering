#include "OBJParser.h"
#include <iostream>
#include <regex>

OBJParser::OBJParser()
{ }

void OBJParser::Read(const std::string& path)
{
	std::cout << "parsing..." << std::endl;

	// vertices and indices
	std::string line;
	std::string originalLine;
	std::string::iterator tempIt;
	m_Input.open(path, std::ios::in);
	if (m_Input.is_open())
	{
		while (!m_Input.eof())
		{
			std::getline(m_Input, line);

			switch (line[0])
			{
			case '#':
				// ignore comments
				break;

			case 'v':

				originalLine = line;
				line.erase(line.begin(), line.begin() + 3);
				Elite::FPoint3 float3;

				// x
				tempIt = { std::find(line.begin(), line.end(), ' ') };
				float3.x = std::stof(std::string{ line.begin(), tempIt });
				line.erase(line.begin(), tempIt + 1);

				// y
				tempIt = std::find(line.begin(), line.end(), ' ');
				float3.y = std::stof(std::string{ line.begin(), tempIt });
				line.erase(line.begin(), tempIt + 1);

				// z
				float3.z = std::stof(std::string{ line.begin(), line.end() });

				line.clear();
				
				if (originalLine[1] == 't') m_VertexUVs.push_back(Elite::FVector3(float3));
				else if (originalLine[1] == 'n') m_VertexNormals.push_back(Elite::FVector3(float3));
				else m_VertexPositions.push_back(float3);

				break;

			case 'f':

				line.erase(line.begin(), line.begin() + 2);
				Vertex vertex{ };
				std::string::iterator stringIt{ };
				unsigned int idx{ };
				std::regex expression{ "(\\d+)" };
				std::smatch match;

				
				for (unsigned int index = 0; index < 3; ++index)
				{
					std::regex_search(line, match, expression);
					idx = std::stoi(match.str(0)) - 1;
						m_IndexBuffer.push_back(idx);
					vertex.position = m_VertexPositions[idx];
					stringIt = std::find(line.begin(), line.end(), '/');
					line.erase(line.begin(), stringIt + 1);

					std::regex_search(line, match, expression);
					idx = std::stoi(match.str(0)) - 1;
					vertex.uv = m_VertexUVs[idx].xy;
					stringIt = std::find(line.begin(), line.end(), '/');
					line.erase(line.begin(), stringIt + 1);

					std::regex_search(line, match, expression);
					idx = std::stoi(match.str(0)) - 1;
					vertex.normal = m_VertexNormals[idx];
					stringIt = std::find(line.begin(), line.end(), ' ');
					line.erase(line.begin(), stringIt + 1);
						m_VertexBuffer.push_back(vertex);
				}
				break;
			}
		}
	}

	//// tangents
	//for (uint32_t i = 0; i < m_IndexBuffer.size(); i += 3)
	//{
	//	uint32_t index0 = m_IndexBuffer[i];
	//	uint32_t index1 = m_IndexBuffer[i + 1];
	//	uint32_t index2 = m_IndexBuffer[i + 2];

	//	const Elite::FPoint3& p0 = m_VertexBuffer[index0].position.xyz;
	//	const Elite::FPoint3& p1 = m_VertexBuffer[index1].position.xyz;
	//	const Elite::FPoint3& p2 = m_VertexBuffer[index2].position.xyz;
	//	const Elite::FVector2& uv0 = m_VertexBuffer[index0].uv;
	//	const Elite::FVector2& uv1 = m_VertexBuffer[index1].uv;
	//	const Elite::FVector2& uv2 = m_VertexBuffer[index2].uv;

	//	const Elite::FVector3 edge0 = p1 - p0;
	//	const Elite::FVector3 edge1 = p2 - p0;
	//	const Elite::FVector2 diffX = Elite::FVector2(uv1.x - uv0.x, uv2.x - uv0.x);
	//	const Elite::FVector2 diffY = Elite::FVector2(uv1.y - uv0.y, uv2.y - uv0.y);
	//	float r = 1.f / Elite::Cross(diffX, diffY);

	//	Elite::FVector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
	//	m_VertexBuffer[index0].tangent += tangent;
	//	m_VertexBuffer[index1].tangent += tangent;
	//	m_VertexBuffer[index2].tangent += tangent;
	//}
	//
	//for (auto& v : m_VertexBuffer)
	//{
	//	v.tangent = Elite::GetNormalized(Elite::Reject(v.tangent, v.normal));
	//}

	std::cout << "parsing finished" << std::endl;
}

std::vector<Vertex>& OBJParser::GetVertexBuffer()
{
	return m_VertexBuffer;
}

std::vector<unsigned int>& OBJParser::GetIndexBuffer()
{
	return m_IndexBuffer;
}

void OBJParser::RemoveDuplicateVertices()
{
	for (std::vector<Elite::FPoint3>::iterator it1 = m_VertexPositions.begin(); it1 != m_VertexPositions.end(); ++it1)
	{
		for (std::vector<Elite::FPoint3>::iterator it2 = m_VertexPositions.begin(); it2 != m_VertexPositions.end(); ++it2)
		{
			if (it1 != it2 && (*it1) == (*it2))
			{
				m_VertexPositions.erase(it1);
			}
		}
	}
}
