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

#include "Vertex.h"

class Cubemap
{
public:
	Cubemap(std::string path);
	~Cubemap();

	void draw();

	Vertex* data = nullptr;
	ID3D11Buffer* data_buffer;

	std::vector<UINT> index_data;
	ID3D11Buffer* index_buffer;

	std::vector<unsigned char*> textures;
	ID3D11Texture2D* cubemap_texture;
	ID3D11ShaderResourceView* cubemap_srv;
};

