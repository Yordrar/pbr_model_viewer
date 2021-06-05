#pragma once

#include <bindable/IBindable.h>
#include <Graphics.h>

class ConstantBuffer : public IBindable
{
public:
	template<typename T>
	ConstantBuffer(Graphics& gfx, T* data, UINT slot);
	~ConstantBuffer();

	virtual void bind(Graphics& gfx) override;

	void update(Graphics& gfx, void const* data, UINT size);

private:
	ID3D11Buffer* m_buffer;
	UINT m_slot;
};

template<typename T>
inline ConstantBuffer::ConstantBuffer(Graphics& gfx, T* data, UINT slot)
	: m_buffer(nullptr)
	, m_slot(slot)
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(T);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subres_data;
	subres_data.pSysMem = data;
	getDevice(gfx)->CreateBuffer(&desc, &subres_data, &m_buffer);
}
