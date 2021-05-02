#pragma once

#include <bindable/IBindable.h>
#include <Graphics.h>

class InputLayout : public IBindable
{
public:
	InputLayout(Graphics& gfx, D3D11_INPUT_ELEMENT_DESC* inputElementDescBuffer, UINT count, ID3DBlob* vertexShaderBytecode);
	~InputLayout();

	virtual void bind(Graphics& gfx) override;

private:
	ID3D11InputLayout* m_inputLayout;
};