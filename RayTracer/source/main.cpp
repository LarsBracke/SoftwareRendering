//External includes
#include "vld.h"
#include "SDL.h"
#include "SDL_surface.h"
#undef main

//Standard includes
#include <iostream>

//Project includes
#include "ETimer.h"
#include "ERenderer.h"

#include "Camera.h"
#include"Object.h"
#include "Ray.h"
#include "Light.h"
#include "LightManager.h"
#include "CameraManager.h"
#include "Material.h"
#include "OBJParser.h"

enum class Scene
{
	Default,
	Bunny
};

static Scene SelectedScene{ Scene::Default };

void HandleSceneSelection();
void CreatePlanes();
void CreateSpheres();
void CreateTriangles();
void CreateBunny();
void CreateCamera(const uint32_t width, const uint32_t height);
void CreateLights();


void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;
	SDL_Window* pWindow = SDL_CreateWindow(
		"RayTracer - Lars Bracke",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	Elite::Timer* pTimer = new Elite::Timer();
	Elite::Renderer* pRenderer = new Elite::Renderer(pWindow);

	//Create Resources
		//Create objects
		HandleSceneSelection();

		//Create camera
		CreateCamera(width, height);

		//Create Lights
		CreateLights();

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool takeScreenshot = false;
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				if(e.key.keysym.scancode == SDL_SCANCODE_X)
					takeScreenshot = true;
				break;
			}
		}
		
		//--------- update ---------
		SceneGraph::GetInstance().UpdateObjects(pTimer->GetElapsed());

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}

		//Save screenshot after full render
		if (takeScreenshot)
		{
			if (!pRenderer->SaveBackbufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
			takeScreenshot = false;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}

void CreatePlanes() 
{
	Object* pObject = new Plane({ 0, 0, 0 }, { 0.25f, 0.25f, 0.25f }, { 0, 1, 0 });
	Material* pMaterial = new Material_Lambert(1.f, pObject->GetColor());
	pObject->SetMaterial(pMaterial);
	SceneGraph::GetInstance().AddObjectToGraph(pObject);

	pObject = new Plane({ 0, 0, -9 }, { 0.25f, 0.25f, 0.25f }, { 0, 0, 1 });
	pMaterial = new Material_Lambert(1.f, pObject->GetColor());
	pObject->SetMaterial(pMaterial);
	SceneGraph::GetInstance().AddObjectToGraph(pObject);
}

void CreateSpheres() 
{
	Object* pObject = new Sphere({ -2.5f, 1.f, 0.f }, { 0.66,0.66f,0.66f }, 1.f);
	Material* pMaterial = new Material_PBR(0.5f, pObject->GetColor(), 0.5f, 1.f, { 252.f / 255.f, 250.f / 255.f, 245.f / 255.f }, true);
	pObject->SetMaterial(pMaterial);
	SceneGraph::GetInstance().AddObjectToGraph(pObject);

	pObject = new Sphere({ 0.f, 1.f, 0.f }, { 0.66,0.66f,0.66f }, 1.f);
	pMaterial = new Material_PBR(0.5f, pObject->GetColor(), 0.5f, 0.6f, { 252.f / 255.f, 250.f / 255.f, 245.f / 255.f }, true);
	pObject->SetMaterial(pMaterial);
	SceneGraph::GetInstance().AddObjectToGraph(pObject);

	pObject = new Sphere({ 2.5,1,0 }, { 0.66,0.66f,0.66f }, 1.f);
	pMaterial = new Material_PBR(0.5f, pObject->GetColor(), 0.5f, 0.1f, { 252.f / 255.f, 250.f / 255.f, 245.f / 255.f }, true);
	pObject->SetMaterial(pMaterial);
	SceneGraph::GetInstance().AddObjectToGraph(pObject);

	pObject = new Sphere({ -2.5, 3.5 ,0 }, { 1.f, 0.f, 0.f }, 1);
	pMaterial = new Material_PBR(0.5f, pObject->GetColor(), 0.5f, 1.f);
	pObject->SetMaterial(pMaterial);
	SceneGraph::GetInstance().AddObjectToGraph(pObject);

	pObject = new Sphere({ 0, 3.5 ,0 }, { 1.f, 0.f, 0.f }, 1);
	pMaterial = new Material_PBR(0.5f, pObject->GetColor(), 0.5f, 0.6f);
	pObject->SetMaterial(pMaterial);
	SceneGraph::GetInstance().AddObjectToGraph(pObject);

	pObject = new Sphere({ 2.5, 3.5 ,0 }, { 1.f, 0.f, 0.f }, 1);
	pMaterial = new Material_PBR(0.5f, pObject->GetColor(), 0.5f, 0.1f);
	pObject->SetMaterial(pMaterial);
	SceneGraph::GetInstance().AddObjectToGraph(pObject);
}

void CreateTriangles() 
{
	Object* pObject = new Triangle({ -0.5, -0.5, 0 }, { -0.5, 0.5, 0 }, { 0.5, -0.5, 0 }, { 1,0,0 }, Cullmode::no_culling);
	pObject->SetTranslation(Elite::FVector3{ -2.5, 5.5, 0 });
	Material* pMaterial = new Material_Lambert(1.f, pObject->GetColor());
	pObject->SetMaterial(pMaterial);
	SceneGraph::GetInstance().AddObjectToGraph(pObject);

	pObject = new Triangle({ -0.5, -0.5, 0 }, { -0.5, 0.5, 0 }, { 0.5, -0.5, 0 }, { 1,0,0 }, Cullmode::front_face);
	pObject->SetTranslation(Elite::FVector3{ 0, 5.5, 0 });
	pMaterial = new Material_Lambert(1.f, pObject->GetColor());
	pObject->SetMaterial(pMaterial);
	SceneGraph::GetInstance().AddObjectToGraph(pObject);

	pObject = new Triangle({ -0.5, -0.5, 0 }, { -0.5, 0.5, 0 }, { 0.5, -0.5, 0 }, { 1,0,0 }, Cullmode::back_face);
	pObject->SetTranslation(Elite::FVector3{ 2.5, 5.5, 0 });
	pMaterial = new Material_Lambert(1.f, pObject->GetColor());
	pObject->SetMaterial(pMaterial);
	SceneGraph::GetInstance().AddObjectToGraph(pObject);
}

void CreateBunny()
{
	OBJParser parser;
	parser.Read("lowpoly_bunny.obj");
	parser.RemoveDuplicateVertices();
	TriangleMesh* pBunny = new TriangleMesh({ 0,2,0 }, { 1,1,1 });
	pBunny->GetVertexBuffer() = parser.GetVertices();
	pBunny->GetIndexBuffer() = parser.GetIndices();
	Material* pMaterial = new Material_LambertPhong(0.5f, pBunny->GetColor(), 0.5f, 60.f);
	pBunny->SetMaterial(pMaterial);
	SceneGraph::GetInstance().AddObjectToGraph(pBunny);
}

void CreateCamera(const uint32_t width, const uint32_t height)
{
	CameraManager::GetInstance().SetCamera(Camera{ {0,3,8 }, 45 , float(width), float(height) });
}

void CreateLights()
{
	Light* pLight = new PointLight{ {1.f, 2.f, 10.f}, {1.f,1.f,1.f}, 100.f };
	LightManager::GetInstance().AddLight(pLight);
	pLight = new PointLight{ {0, 5, -5}, {1.f,1.f,1.f}, 75.f };
	LightManager::GetInstance().AddLight(pLight);
	pLight = new DirectionalLight{ { 0,1,10 }, { 1.f,0.75f,0.75f }, 2.f, { -0.75f, 0.75f, 0.75f } };
	LightManager::GetInstance().AddLight(pLight);
}

void HandleSceneSelection()
{
	SceneGraph::GetInstance().GetObjects().clear();
	switch (SelectedScene)
	{
	case Scene::Default:
		CreatePlanes();
		CreateSpheres();
		CreateTriangles();
		return;
	case Scene::Bunny:
		CreatePlanes();
		CreateBunny();
		return;
	}
}