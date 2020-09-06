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
public:
	static void create(HWND hwnd, int screen_width, int screen_height);
	static void destroy();
	static Graphics* get();
	~Graphics();

	void clear(const FLOAT clear_color[4]);
	void change_fill_mode(D3D11_FILL_MODE mode);
	void present();

	ID3D11VertexShader* create_vertex_shader(std::string filename);
	void set_vertex_shader(ID3D11VertexShader* shader);

	ID3D11PixelShader* create_pixel_shader(std::string filename);
	void set_pixel_shader(ID3D11PixelShader* shader);

	ID3D11Buffer* create_buffer(const void* data, UINT byte_width, D3D11_BIND_FLAG bind_flag);
	void update_buffer(ID3D11Buffer* buffer, const void* data, UINT byte_width);
	void set_buffer(int pos, ID3D11Buffer* buffer);

private:
	Graphics(HWND hwnd, int screen_width, int screen_height);
	static Graphics* instance;

public:
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
	// Vertex input layout
	D3D11_INPUT_ELEMENT_DESC vertex_desc_buffer[3] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ID3D11InputLayout* input_layout;


	ID3D11VertexShader* vertex_shader;
	ID3D11VertexShader* vertex_normal_shader;
	ID3D11PixelShader* pixel_shader;
};

