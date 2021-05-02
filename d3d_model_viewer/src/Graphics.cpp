#include "Graphics.h"

#include "imgui/imgui.h"
#include "imgui/ImGuiFileDialog.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

Graphics::Graphics(HWND hwnd, int screen_width, int screen_height)
{
	// Initialize Direct3D 11
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevel;
	// Specify swap chain settings
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	swap_chain_desc.BufferDesc.Width = screen_width;
	swap_chain_desc.BufferDesc.Height = screen_height;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 0;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.OutputWindow = hwnd;
	swap_chain_desc.Windowed = true;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = 0;
	// Create Direct3D device, context and swap chain
	D3D11CreateDeviceAndSwapChain(
		nullptr, // Default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr, // No software device
		createDeviceFlags,
		nullptr, 0, // Default feature level array
		D3D11_SDK_VERSION, // Direct3D 11 SDK version
		&swap_chain_desc,
		&swap_chain,
		&d3d_device,
		&featureLevel,
		&d3d_context
	);

	// Create depth stencil buffer
	D3D11_TEXTURE2D_DESC depth_stencil_buffer_desc;
	depth_stencil_buffer_desc.Width = screen_width;
	depth_stencil_buffer_desc.Height = screen_height;
	depth_stencil_buffer_desc.MipLevels = 1;
	depth_stencil_buffer_desc.ArraySize = 1;
	depth_stencil_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_buffer_desc.SampleDesc.Count = 1;
	depth_stencil_buffer_desc.SampleDesc.Quality = 0;
	depth_stencil_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_buffer_desc.CPUAccessFlags = 0;
	depth_stencil_buffer_desc.MiscFlags = 0;
	d3d_device->CreateTexture2D(&depth_stencil_buffer_desc, nullptr, &depth_stencil_buffer);
	d3d_device->CreateDepthStencilView(depth_stencil_buffer, nullptr, &depth_stencil_view);

	// Create the render target view
	ID3D11Texture2D* backbuffer;
	swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer));
	d3d_device->CreateRenderTargetView(backbuffer, 0, &render_target_view);
	backbuffer->Release();

	// Bind the render target and depth/stencil buffer to the output merger
	d3d_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

	// Create the viewport and bind it
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)screen_width;
	viewport.Height = (float)screen_height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	d3d_context->RSSetViewports(1, &viewport);

	// Set culling mode to clockwise because we use a right handed coordinate system
	D3D11_RASTERIZER_DESC rasterizer_desc;
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = true;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.DepthClipEnable = true;
	rasterizer_desc.ScissorEnable = false;
	rasterizer_desc.MultisampleEnable = false;
	rasterizer_desc.AntialiasedLineEnable = false;
	d3d_device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state);
	d3d_context->RSSetState(rasterizer_state);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(d3d_device, d3d_context);

	d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

Graphics::~Graphics()
{
	// ImGui cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// Direct3D cleanup
	rasterizer_state->Release();
	render_target_view->Release();
	swap_chain->Release();
	d3d_context->Release();
	d3d_device->Release();
}

void Graphics::clear(const FLOAT clear_color[4])
{
	// Clear render target
	d3d_context->ClearRenderTargetView(render_target_view, clear_color);
	// Clear depth buffer
	d3d_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Graphics::change_fill_mode(D3D11_FILL_MODE mode)
{
	rasterizer_state->Release();
	D3D11_RASTERIZER_DESC rasterizer_desc;
	rasterizer_desc.FillMode = mode;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = true;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.DepthClipEnable = true;
	rasterizer_desc.ScissorEnable = false;
	rasterizer_desc.MultisampleEnable = false;
	rasterizer_desc.AntialiasedLineEnable = false;
	d3d_device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state);
	d3d_context->RSSetState(rasterizer_state);
}

void Graphics::drawIndexed(UINT indexCount)
{
	d3d_context->DrawIndexed(indexCount, 0, 0);
}

void Graphics::present()
{
	swap_chain->Present(1, 0);
}
