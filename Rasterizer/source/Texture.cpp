#include "Texture.h"
#include "SDL_image.h"

#include <iostream>

Texture::Texture(const std::string& path)
	: m_pSurface{ IMG_Load(path.c_str()) }
{ }

Texture::~Texture()
{
	SDL_FreeSurface(m_pSurface);
}

Elite::RGBColor Texture::Sample(const Elite::FVector2 uv) const
{
	Elite::RGBColor color{ };
	Uint32* pPixelBuffer = (Uint32*)m_pSurface->pixels;

	Uint32 xPos{ Uint32(uv.x * m_pSurface->w) };
	Uint32 yPos{ Uint32(uv.y * m_pSurface->h) };

	Uint32 pixel =
	pPixelBuffer[yPos * m_pSurface->w + xPos];
	
	Uint8 red{  };
	Uint8 green{  };
	Uint8 blue{  };
	
	SDL_GetRGB(pixel, m_pSurface->format, &red, &green, &blue);
	color = { float(red / 255.f), float(green / 255.f), float(blue / 255.f) };

	return color;
}
