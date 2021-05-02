#include "VertexBuffer.h"

VertexBuffer::~VertexBuffer()
{
	m_vertexBuffer->Release();
}

void VertexBuffer::bind(Graphics& gfx)
{
	getContext(gfx)->IASetVertexBuffers(0, 1, &m_vertexBuffer, &m_stride, &m_offset);
}
