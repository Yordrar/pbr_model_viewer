#pragma once

#include <Windows.h>
#include <Windowsx.h>

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#include <string>

class Graphics
{
	friend class IBindable;
public:
	Graphics(HWND hwnd, int screen_width, int screen_height);
	~Graphics();

	void clear(const FLOAT clear_color[4]);
	void change_fill_mode(D3D11_FILL_MODE mode);
	void drawIndexed(UINT indexCount);
	void present();

private:
	// Direct3D device, context and swap chain
	ID3D11Device* d3d_device;
	ID3D11DeviceContext* d3d_context;
	IDXGISwapChain* swap_chain;
	// Depth and stencil buffers
	ID3D11Texture2D* depth_stencil_buffer;
	ID3D11DepthStencilView* depth_stencil_view;
	// Render target view of the backbuffer
	ID3D11RenderTargetView* render_target_view;
	// Viewport
	D3D11_VIEWPORT viewport;
	// Rasterizer state
	ID3D11RasterizerState* rasterizer_state;
};

