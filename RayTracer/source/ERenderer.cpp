//External includes
#include "SDL.h"
#include "SDL_surface.h"

#include <iostream>

//Project includes
#include "ERenderer.h"
#include "ERGBColor.h"

#include "EMath.h"
#include "EMathUtilities.h"

#include "Camera.h"
#include"Object.h"
#include "Ray.h"
#include "Light.h"
#include "LightManager.h"
#include "CameraManager.h"
#include "Material.h"
#include "OBJParser.h"

#include <thread>
#include <vector>
#include <numeric>
#include <execution>

Elite::Renderer::Renderer(SDL_Window* pWindow)
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
}

void Elite::Renderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);

	HandleUserInput();
	CameraManager::GetInstance().GetCamera()->HandleCameraMovement(1.f);

	// Reset the pixels
	for (uint32_t r = 0; r < m_Height; ++r)
	{
		for (uint32_t c = 0; c < m_Width; ++c)
		{
			m_pBackBufferPixels[c + (r * m_Width)]
				= SDL_MapRGB(m_pBackBuffer->format, 0, 0, 0);
		}
	}

	// Cast rays
	if (m_UseMultiThreading)
	{
		std::vector<uint32_t> y(m_Height);
		std::iota(y.begin(), y.end(), 0);
		std::for_each(std::execution::par, y.begin(), y.end(),
			[&](uint32_t r)
			{
				for (uint32_t c = 0; c < m_Width; ++c)
				{
					CastRay(c, r);
				}
			});
	}
	else
	{
		for (uint32_t r = 0; r < m_Height; ++r)
		{
			for (uint32_t c = 0; c < m_Width; ++c)
			{
				CastRay(c, r);
			}
		}
	}

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

void Elite::Renderer::HandleUserInput()
{
	const Uint8* pKeyboardState{ SDL_GetKeyboardState(nullptr) };
	if (pKeyboardState[SDL_SCANCODE_Z])
	{
		m_UseHardShadows = !m_UseHardShadows;
	}
}

bool Elite::Renderer::IsShadowRayBlocked(const HitRecord& hitRecord, const Light* pLight, Object* pObject) const
{
	Ray shadowRay{};
	shadowRay.direction = (pLight->GetPosition() - hitRecord.hitPoint);
	shadowRay.tMin = 0.01f;
	shadowRay.tMax = Elite::Normalize(shadowRay.direction);
	shadowRay.origin = hitRecord.hitPoint;

	HitRecord shadowHitRecord;

	// check if the shadowRay hits another object
	for (Object* pOtherObject : SceneGraph::GetInstance().GetObjects())
	{
		if (pObject != pOtherObject)
		{
			if (pOtherObject->Hit(shadowRay, shadowHitRecord, true))
			{
				return true;
			}
		}
	}
	return false;
}

void Elite::Renderer::CastRay(uint32_t c, uint32_t r)
{
	// convert to screen space (in pixel centers)
	float xScreenSpace{ float(2 * ((c + 0.5) / m_Width) - 1) };
	float yScreenSpace{ float(1 - 2 * ((r + 0.5) / m_Height)) };

	// spawn ray in screen space
	Ray ray{ { 0.f,0.f,0.f }, {0.f,0.f,0.f}, 0.f, FLT_MAX };

	// convert to camera space
	float xCameraSpace{ xScreenSpace * CameraManager::GetInstance().GetCamera()->GetAspectRatio() * CameraManager::GetInstance().GetCamera()->GetFOV() };
	float yCameraSpace{ yScreenSpace * CameraManager::GetInstance().GetCamera()->GetFOV() };
	Elite::FPoint3 viewPlanePoint{ xCameraSpace, yCameraSpace, -1 };
	ray.direction = Elite::FVector3(viewPlanePoint);
	Elite::Normalize(ray.direction);

	// move ray to position relative to camera
	ray.origin = CameraManager::GetInstance().GetCamera()->GetPosition();
	ray.direction = (CameraManager::GetInstance().GetCamera()->GetLookAtMatrix() * Elite::FVector4{ ray.direction, 0 }).xyz;

	// check object hits
	float tValue{ ray.tMax };
	HitRecord hitRecord;
	for (Object* pObject : SceneGraph::GetInstance().GetObjects())
	{
		if (pObject->Hit(ray, hitRecord))
		{
			if (hitRecord.tValue < tValue)	// checking for the smallest t value
			{
				Elite::RGBColor finalColor;
				for (Light* pLight : LightManager::GetInstance().GetLights())
				{
					// if there is no object blocking the light,
					// contribute light to final color
					bool castShadow;
					if (m_UseHardShadows) castShadow = IsShadowRayBlocked(hitRecord, pLight, pObject);
					else castShadow = false;

					if (!castShadow)
					{
						Elite::FVector3 lightDirection{ pLight->GetDirection(hitRecord.hitPoint) };
						Elite::Normalize(lightDirection);
						Elite::FVector3 normal{ pObject->GetNormal(hitRecord.hitPoint) };
						Elite::Normalize(normal);
						;
						float dot{ Elite::Dot(normal, lightDirection) };

						if (dot >= 0.f)
						{
							Elite::FVector3 viewDirection{ CameraManager::GetInstance().GetCamera()->GetPosition() - hitRecord.hitPoint };
							Elite::Normalize(viewDirection);

							finalColor += pLight->CalculateColor(hitRecord.hitPoint)
								* pObject->GetMaterial()->Shade(hitRecord, lightDirection, viewDirection)
								* dot;
						}
					}
				}

				finalColor.MaxToOne();
				finalColor *= 255;
				m_pBackBufferPixels[c + (r * m_Width)]
					= SDL_MapRGB(m_pBackBuffer->format,
						Uint8(finalColor.r),
						Uint8(finalColor.g),
						Uint8(finalColor.b));

				tValue = hitRecord.tValue;
			}
			tValue = hitRecord.tValue;
		}
	}
}


