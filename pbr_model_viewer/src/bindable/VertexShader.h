#pragma once

#include <d3d11.h>

#include <bindable/IBindable.h>
#include <Graphics.h>

class VertexShader : public IBindable
{
public:
	VertexShader(Graphics& gfx, std::string filename);
	~VertexShader();

	virtual void bind(Graphics& gfx) override;

	ID3DBlob* getBytecode() const { return m_shaderBytecode; }

private:
	ID3D11VertexShader* m_shader;
	ID3DBlob* m_shaderBytecode;
};