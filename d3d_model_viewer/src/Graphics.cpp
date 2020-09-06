#include "Graphics.h"

Graphics* Graphics::instance = nullptr;

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

	// Create vertex normals shader
	vertex_shader = create_vertex_shader("mesh_vs.cso");
	set_vertex_shader(vertex_shader);

	// Create vertex normals shader
	vertex_normal_shader = create_vertex_shader("normal_vs.cso");

	// Create and set pixel shader
	pixel_shader = create_pixel_shader("PixelShader.cso");
	set_pixel_shader(pixel_shader);
}

void Graphics::create(HWND hwnd, int screen_width, int screen_height)
{
	instance = new Graphics(hwnd, screen_width, screen_height);
}

void Graphics::destroy()
{
	delete instance;
}

Graphics* Graphics::get()
{
	return instance;
}

Graphics::~Graphics()
{
	// Direct3D cleanup
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

void Graphics::present()
{
	swap_chain->Present(1, 0);
}

ID3D11VertexShader* Graphics::create_vertex_shader(std::string filename)
{
	// Create and bind vertex input layout and read vertex shader
	ID3DBlob* vertex_shader_blob;
	std::wstring stemp = std::wstring(filename.begin(), filename.end());
	LPCWSTR sw = stemp.c_str();
	D3DReadFileToBlob(sw, &vertex_shader_blob);
	if (input_layout) input_layout->Release();
	d3d_device->CreateInputLayout(vertex_desc_buffer,
		sizeof(vertex_desc_buffer) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		vertex_shader_blob->GetBufferPointer(),
		vertex_shader_blob->GetBufferSize(), &input_layout);
	d3d_context->IASetInputLayout(input_layout);

	// Create and set vertex shader
	ID3D11VertexShader* vertex_shader;
	d3d_device->CreateVertexShader(vertex_shader_blob->GetBufferPointer(),
		vertex_shader_blob->GetBufferSize(),
		nullptr,
		&vertex_shader);

	return vertex_shader;
}

void Graphics::set_vertex_shader(ID3D11VertexShader* shader)
{
	d3d_context->VSSetShader(shader, nullptr, 0);
}

ID3D11PixelShader* Graphics::create_pixel_shader(std::string filename)
{
	ID3DBlob* pixel_shader_blob;
	std::wstring stemp = std::wstring(filename.begin(), filename.end());
	LPCWSTR sw = stemp.c_str();
	D3DReadFileToBlob(sw, &pixel_shader_blob);
	ID3D11PixelShader* pixel_shader;
	d3d_device->CreatePixelShader(pixel_shader_blob->GetBufferPointer(), pixel_shader_blob->GetBufferSize(), nullptr, &pixel_shader);

	return pixel_shader;
}

void Graphics::set_pixel_shader(ID3D11PixelShader* shader)
{
	d3d_context->PSSetShader(pixel_shader, nullptr, 0);
}

ID3D11Buffer* Graphics::create_buffer(const void* data, UINT byte_width, D3D11_BIND_FLAG bind_flag)
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = byte_width;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = bind_flag;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subres_data;
	subres_data.pSysMem = data;
	ID3D11Buffer* buffer;
	d3d_device->CreateBuffer(&desc, &subres_data, &buffer);

	return buffer;
}

void Graphics::update_buffer(ID3D11Buffer* buffer, const void* data, UINT size)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	//  Disable GPU access to the vertex buffer data.
	d3d_context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//  Update the vertex buffer here.
	memcpy(mappedResource.pData, data, size);
	//  Reenable GPU access to the vertex buffer data.
	d3d_context->Unmap(buffer, 0);
	d3d_context->UpdateSubresource(buffer, 0, nullptr, data, 0, 0);
}

void Graphics::set_buffer(int pos, ID3D11Buffer* buffer)
{
	d3d_context->VSSetConstantBuffers(pos, 1, &buffer);
}
