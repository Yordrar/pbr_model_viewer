#include "Texture.h"

#include <stb_image.h>

Texture::Texture(Graphics& gfx, std::string filename, UINT slot)
	: m_texture(nullptr)
	, m_srv(nullptr)
	, m_slot(slot)
{
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 4);

	D3D11_TEXTURE2D_DESC texture_desc = {};
	texture_desc.Width = width;
	texture_desc.Height = height;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture_desc.SampleDesc = { 1, 0 };
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subres_data;
	subres_data.pSysMem = data;
	subres_data.SysMemPitch = width * 4;
	subres_data.SysMemSlicePitch = 0;

	getDevice(gfx)->CreateTexture2D(&texture_desc, &subres_data, &m_texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texture_desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D = { 0, 1 };

	getDevice(gfx)->CreateShaderResourceView(m_texture, &srvDesc, &m_srv);
}

Texture::~Texture()
{
	m_srv->Release();
	m_texture->Release();
}

void Texture::bind(Graphics& gfx)
{
	getContext(gfx)->PSSetShaderResources(m_slot, 1, &m_srv);
}
