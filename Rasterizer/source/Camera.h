#pragma once
#include "EMath.h"

struct ONB
{
	Elite::FVector3 forward;
	Elite::FVector3 up;
	Elite::FVector3 right;
};

class Camera final
{
public:
	Camera(Elite::FPoint3 position, float fovInDegrees, float screenWidth, float screenHeight);
	~Camera() = default;

	void HandleCameraMovement(float deltaT);

	Elite::FMatrix4 GetCameraToWorldMatrix() const;
	Elite::FMatrix4 GetProjectionMatrix() const;
	Elite::FPoint3 GetPosition() const;
	float GetAspectRatio() const;
	float GetFOV() const;
	ONB GetONB() const;

private:
	void ReconstructONB();
	void RotateCamera(Elite::FVector3 rotation);
	void TranslateCamera(Elite::FVector3 translation);

	Elite::FPoint3 m_Position;
	float m_FOV;
	float m_AspectRatio;
	ONB m_ONB;

	float m_TranslationSpeed;
	float m_RotationSpeed;
	float m_FOVSpeed;

	float m_NearPlane;
	float m_FarPlane;
};

