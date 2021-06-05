#pragma once

#include <Graphics.h>

class IBindable
{
public:
	virtual ~IBindable() = default;

	virtual void bind(Graphics& gfx) = 0;

protected:
	static ID3D11Device* getDevice(Graphics& gfx) { return gfx.d3d_device; }
	static ID3D11DeviceContext* getContext(Graphics& gfx) { return gfx.d3d_context; }
};