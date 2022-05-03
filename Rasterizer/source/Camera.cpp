#include "Camera.h"
#include "SDL.h"

#include <iostream>

Camera::Camera(Elite::FPoint3 position, float fovInDegrees, float screenWidth, float screenHeight)
	: m_Position{ position }
	, m_FOV{ tanf(Elite::ToRadians(fovInDegrees) / 2.f) }
	, m_AspectRatio{ screenWidth / screenHeight }
	, m_TranslationSpeed{ 3.5f }
	, m_RotationSpeed{ 0.8f }
	, m_FOVSpeed{ 0.03f }
	, m_NearPlane{ 0.1f }
	, m_FarPlane{ 1000.f }
{
	m_ONB.forward = { 0,0,1 };
	ReconstructONB();
}

void Camera::HandleCameraMovement(float deltaT)
{
	// change FOV
	const Uint8* pKeyboardState{ SDL_GetKeyboardState(nullptr) };
	if (pKeyboardState[SDL_SCANCODE_KP_PLUS])
	{
		m_FOV += m_FOVSpeed * deltaT;
		std::cout << "FOV: " << m_FOV << std::endl;
	}
	if (pKeyboardState[SDL_SCANCODE_KP_MINUS])
	{
		m_FOV -= m_FOVSpeed * deltaT;
		std::cout << "FOV: " << m_FOV << std::endl;
	}

	// translate camera with keyboard
	Elite::FVector3 translation;
	if (pKeyboardState[SDL_SCANCODE_W])
	{
		translation = -m_ONB.forward;
		translation *= (m_TranslationSpeed * deltaT);
		TranslateCamera(translation);
	}
	if (pKeyboardState[SDL_SCANCODE_S])
	{
		translation = m_ONB.forward;
		translation *= (m_TranslationSpeed * deltaT);
		TranslateCamera(translation);
	}
	if (pKeyboardState[SDL_SCANCODE_D])
	{
		translation = m_ONB.right;
		translation *= (m_TranslationSpeed * deltaT);
		TranslateCamera(translation);
	}
	if (pKeyboardState[SDL_SCANCODE_A])
	{
		translation = -m_ONB.right;
		translation *= (m_TranslationSpeed * deltaT);
		TranslateCamera(translation);
	}

	// translate and rotate camera with mouse
	int xMouse, yMouse;
	int& i1 = xMouse;
	int& i2 = yMouse;
	const Uint32 mouseState{ SDL_GetRelativeMouseState(&i1, &i2) };

	Elite::FVector3 rotation;
	if (mouseState == (SDL_BUTTON(SDL_BUTTON_LEFT) + SDL_BUTTON(SDL_BUTTON_RIGHT)))
	{
		if (xMouse > 0.f)
		{
			translation = m_ONB.right;
			TranslateCamera(translation);
		}
		else if (xMouse < 0.f)
		{
			translation = -m_ONB.right;
			TranslateCamera(translation);
		}
		if (yMouse > 0)
		{
			translation = m_ONB.up;
			TranslateCamera(translation);

		}
		else if (yMouse < 0)
		{
			translation = -m_ONB.up;
			TranslateCamera(translation);
		}
		return;
	}
	else if (mouseState == SDL_BUTTON(1))
	{
		if (xMouse > 0.f)
		{
			Elite::FVector3 rotation{ 0.f, m_RotationSpeed * deltaT, 0.f };
			RotateCamera(rotation);
		}
		else if (xMouse < 0.f)
		{
			Elite::FVector3 rotation{ 0.f, -m_RotationSpeed * deltaT, 0.f };
			RotateCamera(rotation);
		}
		if (yMouse > 0)
		{
			Elite::FVector3 translation{ -m_ONB.forward };
			translation *= (m_TranslationSpeed * deltaT);
			TranslateCamera(translation);
		}
		else if (yMouse < 0)
		{
			Elite::FVector3 translation{ m_ONB.forward };
			translation *= (m_TranslationSpeed * deltaT);
			TranslateCamera(translation);
		}
		return;
	}
	else if (mouseState == SDL_BUTTON(3))
	{
		if (xMouse > 0.f)
		{
			rotation = { 0.f, -m_RotationSpeed * deltaT, 0.f };
			RotateCamera(rotation);
		}
		if (xMouse < 0.f)
		{
			rotation = { 0.f, m_RotationSpeed * deltaT, 0.f };
			RotateCamera(rotation);
		}
		if (yMouse > 0)
		{
			rotation = { -m_RotationSpeed * deltaT, 0.f, 0.f };
			RotateCamera(rotation);
		}
		if (yMouse < 0)
		{
			rotation = { m_RotationSpeed * deltaT, 0.f, 0.f };
			RotateCamera(rotation);
		}
		return;
	}
}

void Camera::RotateCamera(Elite::FVector3 rotation)
{
	Elite::FMatrix3 rotationMatrix{ Elite::MakeRotationZYX(rotation.x, rotation.y, rotation.z) };
	m_ONB.forward = rotationMatrix * m_ONB.forward;
	ReconstructONB();
}

void Camera::TranslateCamera(Elite::FVector3 translation)
{
	m_Position += translation;
}

float Camera::GetAspectRatio() const
{
	return m_AspectRatio;
}

Elite::FMatrix4 Camera::GetCameraToWorldMatrix() const
{
	return
		Elite::FMatrix4
	{
		m_ONB.right.x,	m_ONB.up.x, m_ONB.forward.x,	m_Position.x,
		m_ONB.right.y,	m_ONB.up.y, m_ONB.forward.y,	m_Position.y,
		m_ONB.right.z,	m_ONB.up.z, m_ONB.forward.z,	m_Position.z,
		0,				0,			0,					1
	};
}

Elite::FMatrix4 Camera::GetProjectionMatrix() const
{
	return
		Elite::FMatrix4
	{
		1.f / (m_AspectRatio * m_FOV),		0.f,						0.f,													0.f,
		0.f,								1.f / m_FOV,				0.f,													0.f,
		0.f,								0.f,						(m_FarPlane) / (m_NearPlane - m_FarPlane),				(m_FarPlane * m_NearPlane) / (m_NearPlane - m_FarPlane),
		0.f,								0.f,						-1.f,													0.f
	};
}

Elite::FPoint3 Camera::GetPosition() const
{
	return m_Position;
}

float Camera::GetFOV() const
{
	return m_FOV;
}

ONB Camera::GetONB() const
{
	return m_ONB;
}

void Camera::ReconstructONB()
{
	Elite::FVector3 worldUp{ 0,1,0 };

	m_ONB.right = Elite::Cross(worldUp, m_ONB.forward);
	m_ONB.up = Elite::Cross(m_ONB.forward, m_ONB.right);

	Elite::Normalize(m_ONB.forward);
	Elite::Normalize(m_ONB.right);
	Elite::Normalize(m_ONB.up);
}