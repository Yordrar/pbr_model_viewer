#include "TextureSampler.h"

TextureSampler::TextureSampler(Graphics& gfx, UINT slot, D3D11_FILTER filter)
	: m_samplerState(nullptr)
	, m_slot(slot)
{
	D3D11_SAMPLER_DESC texture_sampler_desc = {};
	texture_sampler_desc.Filter = filter;
	texture_sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	texture_sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	texture_sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	texture_sampler_desc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	texture_sampler_desc.MipLODBias = 0.0f;
	texture_sampler_desc.MaxAnisotropy = 1;
	texture_sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	texture_sampler_desc.MinLOD = 0.0f;
	texture_sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	getDevice(gfx)->CreateSamplerState(&texture_sampler_desc, &m_samplerState);
}

TextureSampler::~TextureSampler()
{
	m_samplerState->Release();
}

void TextureSampler::bind(Graphics& gfx)
{
	getContext(gfx)->PSSetSamplers(m_slot, 1, &m_samplerState);
}
