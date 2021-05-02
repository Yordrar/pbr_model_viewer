#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(Graphics& gfx, UINT* indices, UINT count)
	: m_indexBuffer(nullptr)
	, m_indexCount(count)
{
	D3D11_BUFFER_DESC vertex_indices_desc;
	vertex_indices_desc.ByteWidth = m_indexCount * sizeof(UINT);
	vertex_indices_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vertex_indices_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vertex_indices_desc.CPUAccessFlags = 0;
	vertex_indices_desc.MiscFlags = 0;
	vertex_indices_desc.StructureByteStride = sizeof(UINT);

	D3D11_SUBRESOURCE_DATA vertex_indices_subresource_data;
	vertex_indices_subresource_data.pSysMem = indices;


	getDevice(gfx)->CreateBuffer(&vertex_indices_desc, &vertex_indices_subresource_data, &m_indexBuffer);
}

IndexBuffer::~IndexBuffer()
{
	m_indexBuffer->Release();
}

void IndexBuffer::bind(Graphics& gfx)
{
	getContext(gfx)->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}
