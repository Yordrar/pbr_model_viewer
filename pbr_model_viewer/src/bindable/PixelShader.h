#pragma once

#include <d3d11.h>

#include <bindable/IBindable.h>
#include <Graphics.h>

class PixelShader : public IBindable
{
public:
	PixelShader(Graphics& gfx, std::string filename);
	~PixelShader();

	virtual void bind(Graphics& gfx) override;

	ID3DBlob* getBytecode() const { return m_shaderBytecode; }

private:
	ID3D11PixelShader* m_shader;
	ID3DBlob* m_shaderBytecode;
};