#pragma once

#include <bindable/IBindable.h>
#include <Graphics.h>

class TextureSampler : public IBindable
{
public:
	TextureSampler(Graphics& gfx, UINT slot);
	~TextureSampler();

	virtual void bind(Graphics& gfx) override;

private:
	ID3D11SamplerState* m_samplerState;
	UINT m_slot;
};

