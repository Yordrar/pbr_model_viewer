#pragma once

#include <bindable/IBindable.h>
#include <Graphics.h>

class Texture : public IBindable
{
public:
	Texture(Graphics& gfx, std::string filename, UINT slot);
	~Texture();

	virtual void bind(Graphics& gfx) override;

private:
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_srv;
	UINT m_slot;
};