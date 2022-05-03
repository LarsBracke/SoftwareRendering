//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERenderer.h"


Elite::Renderer::Renderer(SDL_Window * pWindow)
{
	//Initialize
	m_pWindow = pWindow;
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	CreateObjects();
	m_DepthBuffer = std::vector<float>(uint64_t(m_Width) * uint64_t(m_Height), FLT_MAX);

	CameraManager::GetInstance().SetCamera(Camera({ {0.f, 1.f, 45.f}, 60.f, float(m_Width), float(m_Height) }));
}

void Elite::Renderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);
	// reset the pixels
	for (uint32_t r = 0; r < m_Height; ++r)
	{
		for (uint32_t c = 0; c < m_Width; ++c)
		{
			m_pBackBufferPixels[c + (r * m_Width)]
				= SDL_MapRGB(m_pBackBuffer->format, Uint8(50), Uint8(50), Uint8(50));
		}
	}

	Camera* pCamera =
		CameraManager::GetInstance().GetCamera();
	

	for (Object* pObject : SceneGraph::GetInstance().GetObjects())
	{
		// vertex transformations
		std::vector<Vertex> transformedVertices;

		bool areVerticesInsideFrustum =
		VertexTransformationFunction(pObject->GetVertices(), transformedVertices,
			pCamera->GetCameraToWorldMatrix(), pCamera->GetProjectionMatrix());

		if (!areVerticesInsideFrustum) continue;

		ConvertNDCToRaster(transformedVertices);

		// create bounding box
		Elite::FPoint2 topLeft
			= GetTopLeftFromVertices(transformedVertices);
		Elite::FPoint2 bottomRight
			= GetBottomRightFromVertices(transformedVertices);

		// create veriables
		Elite::RGBColor finalColor{ 0.f, 0.f, 0.f };
		static Texture diffuse{ "Resources/vehicle_diffuse.png" };
		//static Texture diffuse{ "Resources/tuktuk.png" };
		//static Texture diffuse{ "Resources/uv_grid.png" };
		//static Texture diffuse{ "Resources/uv_grid_2.png" };
		static Texture normal{ "Resources/vehicle_normal.png" };

		// loop over all the pixels and calculate color
		for (uint32_t r = (bottomRight.y); r < (topLeft.y); ++r)
		{
			for (uint32_t c = (topLeft.x); c < (bottomRight.x); ++c)
			{
				VertexRecord vertexRecord;
				if (pObject->Hit(Elite::FPoint2{ float(c), float(r) }, transformedVertices, vertexRecord))
				{
					unsigned int pixelIndex{ c + (r * m_Width) };

					float depthValue = vertexRecord.depthInterpolated;
					if (depthValue >= m_DepthBuffer[pixelIndex]) continue;
					else
					{
						m_DepthBuffer[pixelIndex] = depthValue;

						vertexRecord.uvInterpolated = vertexRecord.uvInterpolated * depthValue;
						vertexRecord.uvInterpolated.y = 1 - vertexRecord.uvInterpolated.y;
						vertexRecord.normalInterpolated *= depthValue;

						Vertex outVertex{ };
						outVertex.normal = vertexRecord.normalInterpolated;
						outVertex.tangent = vertexRecord.tangentInterpolated;
						outVertex.uv = vertexRecord.uvInterpolated;

						finalColor = diffuse.Sample(outVertex.uv);
						finalColor += PixelShading(outVertex, finalColor);

						finalColor *= 255;
						m_pBackBufferPixels[c + (r * m_Width)]
							= SDL_MapRGB(m_pBackBuffer->format,
								Uint8(finalColor.r),
								Uint8(finalColor.g),
								Uint8(finalColor.b));
					}
				}
			}
		}
	}

	m_DepthBuffer = std::vector<float>(uint64_t(m_Width) * uint64_t(m_Height), FLT_MAX);

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

bool Elite::Renderer::VertexTransformationFunction(const std::vector<Vertex>& originalVertices, std::vector<Vertex>& transformedVertices, Elite::FMatrix4 cameraToWorldMatrix, Elite::FMatrix4 viewProjectionMatrix)
{
	for (const Vertex& originalVertex : originalVertices)
	{
		// transform vertex
		Vertex transformedVertex = 
			{ viewProjectionMatrix * Elite::Inverse(cameraToWorldMatrix) * originalVertex.position, originalVertex.uv, originalVertex.color };
		transformedVertex.normal = originalVertex.normal;
		transformedVertex.tangent = originalVertex.tangent;

		transformedVertex.position.x /= transformedVertex.position.w;
		transformedVertex.position.y /= transformedVertex.position.w;
		transformedVertex.position.z /= transformedVertex.position.w;

		transformedVertices.push_back(transformedVertex);

		// culling
		if (transformedVertex.position.x > 1.f || transformedVertex.position.y > 1.f
			|| transformedVertex.position.x < -1.f || transformedVertex.position.y < -1.f)
		{
			//return false;
		}
	}
	return true;
}

void Elite::Renderer::VertexTransformationFuntion(const std::vector<Vertex> originalVertices,
	std::vector<Vertex>& transformedVertices, const Elite::FMatrix4& cameraToWorld,
	uint32_t width, uint32_t height, float fovAngle)
{
	for (const Vertex& vertex : originalVertices)
	{
		// create new vertices
		Elite::FPoint3 cameraSpaceVertex;
		Elite::FPoint3 projectedVertex;

		// convert vertex to cameraspace
		Elite::FMatrix4 worldToCamera{ Elite::Inverse(cameraToWorld) };
		cameraSpaceVertex = (worldToCamera * Elite::FPoint4(vertex.position.xyz, 0.f)).xyz;

		// convert to NDC?
		projectedVertex.x = cameraSpaceVertex.x / (-cameraSpaceVertex.z);
		projectedVertex.y = cameraSpaceVertex.y / (-cameraSpaceVertex.z);
		projectedVertex.z = -cameraSpaceVertex.z;

		// apply camera settings
		projectedVertex.x = projectedVertex.x / (float(width) / float(height) * fovAngle);
		projectedVertex.y = projectedVertex.y / fovAngle;


		Vertex newVertex{ projectedVertex, vertex.uv, vertex.color };
		transformedVertices.push_back(newVertex);
	}
}

void Elite::Renderer::ConvertNDCToRaster(Vertex& vertex)
{
	vertex.position.x = ((vertex.position.x + 1) / 2.f) * float(m_Width);
	vertex.position.y = ((1 - vertex.position.y) / 2.f) * float(m_Height);
}

void Elite::Renderer::ConvertNDCToRaster(std::vector<Vertex>& vertices)
{
	for (Vertex& vertex : vertices)
	{
		vertex.position.x = ((vertex.position.x + 1) / 2.f) * float(m_Width);
		vertex.position.y = ((1 - vertex.position.y) / 2.f) * float(m_Height);
	}
}

Elite::FPoint2 Elite::Renderer::GetTopLeftFromVertices(const std::vector<Vertex>& vertices)
{
	Elite::FPoint2 topLeft{ FLT_MAX, 0.f };
	for (const Vertex& vertex : vertices)
	{
		topLeft.x = std::min(topLeft.x, vertex.position.x);
		topLeft.y = std::max(topLeft.y, vertex.position.y);
	}

	topLeft.x = Clamp(float(topLeft.x), 0.f, float(m_Width));
	topLeft.y = Clamp(float(topLeft.y), 0.f, float(m_Height));

	return topLeft;
}

Elite::FPoint2 Elite::Renderer::GetBottomRightFromVertices(const std::vector<Vertex>& vertices)
{
	Elite::FPoint2 bottomRight{ 0.f, FLT_MAX };
	for (const Vertex& vertex : vertices)
	{
		bottomRight.x = std::max(bottomRight.x, vertex.position.x);
		bottomRight.y = std::min(bottomRight.y, vertex.position.y);
	}

	bottomRight.x = Clamp(float(bottomRight.x), 0.f, float(m_Width));
	bottomRight.y = Clamp(float(bottomRight.y), 0.f, float(m_Height));

	return bottomRight;
}

Elite::RGBColor Elite::Renderer::PixelShading(const Vertex& outVertex, const Elite::RGBColor& diffuseColor)
{
	Elite::RGBColor color;
	DirectionalLight light{ {1,1,1}, 0.5f, {0.577f, -0.577f, -0.577f} };

	//### doens't work ###
	//Elite::FVector3 binormal = Elite::Cross(outVertex.tangent, outVertex.normal);
	//Elite::FMatrix3 tangentSpaceAxis = Elite::FMatrix3(outVertex.tangent, binormal, outVertex.normal);
	//Elite::FVector3 newNormal = tangentSpaceAxis * outVertex.normal;

	float observedArea = Elite::Dot((-outVertex.normal), light.GetDirection());

	color = light.GetColor() * light.GetIntensity() * diffuseColor * observedArea;
	return color;
}

void Elite::Renderer::CreateObjects()
{
	// ##### Triangles #####

	//Triangle* pTriangle = new Triangle
	//{
	//	{{0.f, 4.f, -2.f}, {1,0,0}},
	//	{{-3.f, -2.f, -2.f}, {0,1,0}},
	//	{{3.f, -2.f, -2.f}, {0,0,1}},
	//	{1,1,1}
	//};
	//SceneGraph::GetInstance().AddObjectToGraph(pTriangle);

	//pTriangle = new Triangle
	//{
	//	{{0.f, 2.f, -1.f}, {1,1,1}},
	//	{{-1.f, 0.f, -1.f}, {1,1,1}},
	//	{{1.f, 0.f, -1.f}, {1,1,1}},
	//	{1,1,1}
	//};
	//SceneGraph::GetInstance().AddObjectToGraph(pTriangle);


	// ##### quad #####

	//std::vector<Vertex> vertexBuffer{ Vertex{{-3,3,-2} , {0,0}}, Vertex{{0,3,-2}, {0.5f,0}}, Vertex{{3,3,-2}, {1,0}},
	//Vertex{{-3,0,-2}, {0,0.5f}}, Vertex{{0,0,-2}, {0.5f,0.5f}}, Vertex{{3,0,-2}, {1, 0.5f}}, Vertex{{-3,-3,-2}, {0,1}}, 
	//Vertex{{0,-3,-2}, {0.5f,1}}, Vertex{{3,-3,-2}, {1,1}} };
	//std::vector<unsigned int> indexBuffer =
	//{ 0,3,1,3,4,1,1,4,2,4,5,2,3,6,4,6,7,4,4,7,5,7,8,5 };
	//Quad* pQuad = new Quad(vertexBuffer, indexBuffer, indexBuffer);
	//SceneGraph::GetInstance().AddObjectToGraph(pQuad);





	// ##### meshes #####

	Object* pObj = nullptr;
	std::vector<Vertex> vertexBuffer;
	std::vector<unsigned int> indexBuffer;

	//// ### tuktuk ###
	//OBJParser parser;
	//parser.Read("Resources/tuktuk.obj");
	//vertexBuffer = parser.GetVertexBuffer();
	//indexBuffer = parser.GetIndexBuffer();

	//pObj = new Mesh(vertexBuffer, indexBuffer);
	//delete pObj;

	// ### vehicle ###
	OBJParser parser;
	parser.Read("Resources/vehicle.obj");
	vertexBuffer = parser.GetVertexBuffer();
	indexBuffer = parser.GetIndexBuffer();

	pObj = new Mesh(vertexBuffer, indexBuffer);
	delete pObj;	
}
