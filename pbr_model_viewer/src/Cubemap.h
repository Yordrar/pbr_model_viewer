#pragma once

#include <Windows.h>
#include <Windowsx.h>

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#include <string>
#include <vector>

#include <drawable/IDrawable.h>
#include <Graphics.h>

class Cubemap : public IDrawable
{
public:
	Cubemap(Graphics& gfx, std::string path);
	~Cubemap();
};

