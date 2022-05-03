#pragma once
#include "Camera.h"

class CameraManager
{
public:
	~CameraManager();
	static CameraManager& GetInstance();
	Camera* GetCamera();
	void SetCamera(const Camera& camera);
private:
	CameraManager();
	Camera* m_pCamera;
};

