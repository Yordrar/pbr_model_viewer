#pragma once

#include <vector>

#include <bindable/IBindable.h>
#include <Graphics.h>

class VertexBuffer : public IBindable
{
public:
	template<typename T>
	VertexBuffer(Graphics& gfx, T* vertices, UINT count);
	virtual ~VertexBuffer();

	virtual void bind(Graphics& gfx) override;

private:
	ID3D11Buffer* m_vertexBuffer;
	UINT m_stride;
	UINT m_offset;
};

template<typename T>
inline VertexBuffer::VertexBuffer(Graphics& gfx, T* vertices, UINT count)
	: m_vertexBuffer(nullptr)
	, m_stride(sizeof(T))
	, m_offset(0)
{
	D3D11_BUFFER_DESC vertex_buffer_desc;
	vertex_buffer_desc.ByteWidth = count * sizeof(T);
	vertex_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.CPUAccessFlags = 0;
	vertex_buffer_desc.MiscFlags = 0;
	vertex_buffer_desc.StructureByteStride = sizeof(T);

	D3D11_SUBRESOURCE_DATA vertex_subresource_data;
	vertex_subresource_data.pSysMem = vertices;

	getDevice(gfx)->CreateBuffer(&vertex_buffer_desc, &vertex_subresource_data, &m_vertexBuffer);
}
