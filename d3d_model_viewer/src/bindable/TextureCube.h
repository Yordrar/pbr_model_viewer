#pragma once

#include <vector>

#include <stb_image.h>

#include "IBindable.h"
#include <Graphics.h>

class TextureCube : public IBindable
{
public:
	TextureCube(Graphics& gfx, std::string path);
	~TextureCube();

	virtual void bind(Graphics& gfx) override;

private:
	std::vector<unsigned char*> textures;
	ID3D11Texture2D* cubemap_texture;
	ID3D11ShaderResourceView* cubemap_srv;
};

