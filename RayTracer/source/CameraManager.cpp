#include "CameraManager.h"
#include <iostream>

CameraManager::~CameraManager()
{
	std::cout << "CameraManager destructor" << std::endl;
	delete m_pCamera;
	m_pCamera = nullptr;
}

CameraManager& CameraManager::GetInstance()
{
	static CameraManager cameraManager;
	return cameraManager;
}

Camera* CameraManager::GetCamera()
{
	return m_pCamera;
}

void CameraManager::SetCamera(const Camera& camera)
{
	if (m_pCamera != nullptr) delete m_pCamera;
	m_pCamera = new Camera(camera);
}

CameraManager::CameraManager()
	: m_pCamera{ nullptr }
{ }
