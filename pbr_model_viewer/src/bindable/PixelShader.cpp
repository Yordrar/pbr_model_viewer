#include "PixelShader.h"

PixelShader::PixelShader(Graphics& gfx, std::string filename)
	: m_shader(nullptr)
	, m_shaderBytecode(nullptr)
{
	std::wstring stemp = std::wstring(filename.begin(), filename.end());
	LPCWSTR sw = stemp.c_str();
	D3DReadFileToBlob(sw, &m_shaderBytecode);

	getDevice(gfx)->CreatePixelShader(
		m_shaderBytecode->GetBufferPointer(),
		m_shaderBytecode->GetBufferSize(),
		nullptr,
		&m_shader
	);
}

PixelShader::~PixelShader()
{
	m_shader->Release();
}

void PixelShader::bind(Graphics& gfx)
{
	getContext(gfx)->PSSetShader(m_shader, nullptr, 0);
}
