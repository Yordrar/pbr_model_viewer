#pragma once

#include <Windows.h>
#include <Windowsx.h>

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#include "Vertex.h"

class Grid
{
public:
	Grid();
	~Grid();

	void draw();

	Vertex* data = nullptr;
	ID3D11Buffer* data_buffer;
};

