#pragma once
#include <string>
#include "EMath.h"
#include "ERGBColor.h"

struct SDL_Surface;

class Texture final
{
public:
	Texture(const std::string& path);
	~Texture();

	Elite::RGBColor Sample(const Elite::FVector2 uv) const;

private:
	SDL_Surface* m_pSurface;
};

