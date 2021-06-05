#include "TextureCube.h"

#include <stb_image.h>

TextureCube::TextureCube(Graphics& gfx, std::string path, UINT slot )
	: cubemap_texture(nullptr)
	, cubemap_srv(nullptr)
	, m_slot(slot)
{
	int width, height, nrChannels;
	textures.push_back(stbi_load((path + "\\px.png").c_str(), &width, &height, &nrChannels, 4));
	textures.push_back(stbi_load((path + "\\nx.png").c_str(), &width, &height, &nrChannels, 4));
	textures.push_back(stbi_load((path + "\\py.png").c_str(), &width, &height, &nrChannels, 4));
	textures.push_back(stbi_load((path + "\\ny.png").c_str(), &width, &height, &nrChannels, 4));
	textures.push_back(stbi_load((path + "\\pz.png").c_str(), &width, &height, &nrChannels, 4));
	textures.push_back(stbi_load((path + "\\nz.png").c_str(), &width, &height, &nrChannels, 4));

	D3D11_TEXTURE2D_DESC textures_desc = {};
	textures_desc.Width = width;
	textures_desc.Height = height;
	textures_desc.MipLevels = 1;
	textures_desc.ArraySize = 6;
	textures_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textures_desc.SampleDesc = { 1, 0 };
	textures_desc.Usage = D3D11_USAGE_DEFAULT;
	textures_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textures_desc.CPUAccessFlags = 0;
	textures_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	D3D11_SUBRESOURCE_DATA subres_data[6];
	for (int i = 0; i < 6; i++)
	{
		subres_data[i].pSysMem = textures[i];
		subres_data[i].SysMemPitch = width * 4;
		subres_data[i].SysMemSlicePitch = 0;
	}

	getDevice(gfx)->CreateTexture2D(&textures_desc, subres_data, &cubemap_texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textures_desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D = { 0, 1 };

	getDevice(gfx)->CreateShaderResourceView(cubemap_texture, &srvDesc, &cubemap_srv);
}

TextureCube::~TextureCube()
{
	for (auto tex : textures)
	{
		delete tex;
	}
	if ( cubemap_srv ) cubemap_srv->Release();
	if ( cubemap_texture ) cubemap_texture->Release();
}

void TextureCube::bind(Graphics& gfx)
{
	getContext(gfx)->PSSetShaderResources(m_slot, 1, &cubemap_srv);
}
