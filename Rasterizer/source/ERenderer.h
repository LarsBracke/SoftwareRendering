/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render to, does traverse the pixels 
// and traces the rays using a tracer
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include <iostream>
#include <vector>

#include "EMath.h"
#include "ERGBColor.h"

#include "Camera.h"
#include "Vertex.h"
#include "Triangle.h"
#include "Quad.h"
#include "Mesh.h"
#include "Texture.h"
#include "SceneGraph.h"
#include "CameraManager.h"
#include "OBJParser.h"
#include "DirectionalLight.h"

struct SDL_Window;
struct SDL_Surface;

namespace Elite
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render();
		bool SaveBackbufferToImage() const;

	private:
		SDL_Window* m_pWindow = nullptr;
		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		uint32_t* m_pBackBufferPixels = nullptr;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		std::vector<float> m_DepthBuffer;

		bool VertexTransformationFunction(const std::vector<Vertex>& originalVertices, std::vector<Vertex>& transformedVertices,
			Elite::FMatrix4 cameraToWorldMatrix, Elite::FMatrix4 viewProjectionMatrix);
		void VertexTransformationFuntion(const std::vector<Vertex> originalVertices,
			std::vector<Vertex>& transformedVertices, const Elite::FMatrix4& cameraToWorld,
			uint32_t width, uint32_t height, float fovAngle);
		void ConvertNDCToRaster(Vertex& vertex);
		void ConvertNDCToRaster(std::vector<Vertex>& vertices);

		Elite::FPoint2 GetTopLeftFromVertices(const std::vector<Vertex>& vertices);
		Elite::FPoint2 GetBottomRightFromVertices(const std::vector<Vertex>& vertices);

		Elite::RGBColor PixelShading(const Vertex& outVertex, const Elite::RGBColor& diffuseColor);

		void CreateObjects();
	};
}

#endif