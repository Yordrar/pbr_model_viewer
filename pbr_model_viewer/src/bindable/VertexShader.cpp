#include "VertexShader.h"

VertexShader::VertexShader(Graphics& gfx, std::string filename)
{
	std::wstring stemp = std::wstring(filename.begin(), filename.end());
	LPCWSTR sw = stemp.c_str();
	D3DReadFileToBlob(sw, &m_shaderBytecode);

	getDevice(gfx)->CreateVertexShader(
		m_shaderBytecode->GetBufferPointer(),
		m_shaderBytecode->GetBufferSize(),
		nullptr,
		&m_shader
	);
}

VertexShader::~VertexShader()
{
	m_shader->Release();
}

void VertexShader::bind(Graphics& gfx)
{
	getContext(gfx)->VSSetShader(m_shader, nullptr, 0);
}
