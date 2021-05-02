#include "ConstantBuffer.h"

ConstantBuffer::~ConstantBuffer()
{
	m_buffer->Release();
}

void ConstantBuffer::bind(Graphics& gfx)
{
	getContext(gfx)->VSSetConstantBuffers(m_slot, 1, &m_buffer);
}

void ConstantBuffer::update(Graphics& gfx, void const* data, UINT size)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	//  Disable GPU access to the vertex buffer data.
	getContext(gfx)->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//  Update the vertex buffer here.
	memcpy(mappedResource.pData, data, size);
	//  Reenable GPU access to the vertex buffer data.
	getContext(gfx)->Unmap(m_buffer, 0);
}
