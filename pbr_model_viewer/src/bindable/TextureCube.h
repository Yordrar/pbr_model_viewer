#pragma once

#include <vector>

#include "IBindable.h"
#include <Graphics.h>

class TextureCube : public IBindable
{
public:
	TextureCube(Graphics& gfx, std::string path, UINT slot);
	~TextureCube();

	virtual void bind(Graphics& gfx) override;

private:
	std::vector<unsigned char*> textures;
	ID3D11Texture2D* cubemap_texture;
	ID3D11ShaderResourceView* cubemap_srv;
	UINT m_slot;
};

