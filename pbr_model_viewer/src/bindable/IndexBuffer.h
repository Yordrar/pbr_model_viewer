#pragma once

#include <vector>

#include <bindable/IBindable.h>
#include <Graphics.h>

class IndexBuffer : public IBindable
{
public:
	IndexBuffer(Graphics& gfx, UINT* indices, UINT count);
	virtual ~IndexBuffer();

	virtual void bind(Graphics& gfx) override;

	UINT getIndexCount() const { return m_indexCount; }

private:
	ID3D11Buffer* m_indexBuffer;
	UINT m_indexCount;
	UINT m_stride;
	UINT m_offset;
};
