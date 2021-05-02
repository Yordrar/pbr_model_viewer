#include "InputLayout.h"

InputLayout::InputLayout(Graphics& gfx, D3D11_INPUT_ELEMENT_DESC* inputElementDescBuffer, UINT count, ID3DBlob* vertexShaderBytecode)
	: m_inputLayout(nullptr)
{
	getDevice(gfx)->CreateInputLayout(
		inputElementDescBuffer,
		count,
		vertexShaderBytecode->GetBufferPointer(),
		vertexShaderBytecode->GetBufferSize(), 
		&m_inputLayout
	);
}

InputLayout::~InputLayout()
{
	m_inputLayout->Release();
}

void InputLayout::bind(Graphics& gfx)
{
	getContext(gfx)->IASetInputLayout(m_inputLayout);
}
