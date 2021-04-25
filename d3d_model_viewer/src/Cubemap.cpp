#include "Cubemap.h"

#include <string>

#include "Graphics.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Cubemap::Cubemap(std::string path)
{
	// Initialize cubemap
	data = new Vertex[8];

	float size = 100.0f;

	data[0].position = { -size, -size, -size };
	data[1].position = { size, -size, -size };
	data[2].position = { -size, size, -size };
	data[3].position = { size, size, -size };
	data[4].position = { -size, -size, size };
	data[5].position = { size, -size, size };
	data[6].position = { -size, size, size };
	data[7].position = { size, size, size };
	data_buffer = Graphics::get()->create_buffer(data, 8 * sizeof(Vertex), D3D11_BIND_VERTEX_BUFFER);

	index_data = {
				0,1,2, 2,1,3,
				1,5,3, 3,5,7,
				2,3,6, 3,7,6,
				4,7,5, 4,6,7,
				0,2,4, 2,6,4,
				0,4,1, 1,4,5
	};
	index_buffer = Graphics::get()->create_buffer(index_data.data(), 36 * sizeof(UINT), D3D11_BIND_INDEX_BUFFER);

	int width, height, nrChannels;
	textures.push_back(stbi_load((path + "px.png").c_str(), &width, &height, &nrChannels, 4));
	textures.push_back(stbi_load((path + "nx.png").c_str(), &width, &height, &nrChannels, 4));
	textures.push_back(stbi_load((path + "py.png").c_str(), &width, &height, &nrChannels, 4));
	textures.push_back(stbi_load((path + "ny.png").c_str(), &width, &height, &nrChannels, 4));
	textures.push_back(stbi_load((path + "pz.png").c_str(), &width, &height, &nrChannels, 4));
	textures.push_back(stbi_load((path + "nz.png").c_str(), &width, &height, &nrChannels, 4));

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

	Graphics::get()->d3d_device->CreateTexture2D(&textures_desc, subres_data, &cubemap_texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textures_desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D = { 0, 1 };

	Graphics::get()->d3d_device->CreateShaderResourceView(cubemap_texture, &srvDesc, &cubemap_srv);

	Graphics::get()->d3d_context->PSSetShaderResources(0, 1, &cubemap_srv);
}

Cubemap::~Cubemap()
{
	delete data;
	data_buffer->Release();
}

void Cubemap::draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	Graphics::get()->set_vertex_shader(Graphics::get()->cubemap_vertex_shader);
	Graphics::get()->set_pixel_shader(Graphics::get()->cubemap_pixel_shader);
	// Set vertex buffe
	Graphics::get()->d3d_context->IASetVertexBuffers(0, 1, &data_buffer, &stride, &offset);
	// Set primitive topology type
	Graphics::get()->d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Graphics::get()->d3d_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
	Graphics::get()->d3d_context->PSSetShaderResources(0, 1, &cubemap_srv);
	//Draw grid
	Graphics::get()->d3d_context->DrawIndexed(36, 0, 0);
}
