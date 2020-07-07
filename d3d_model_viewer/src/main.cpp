#include <Windows.h>

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <string>

int screen_width = 1280;
int screen_height = 720;

namespace Colors
{
	XMGLOBALCONST DirectX::XMFLOAT4 White = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::XMFLOAT4 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMFLOAT4 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMFLOAT4 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMFLOAT4 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::XMFLOAT4 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMFLOAT4 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::XMFLOAT4 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };
}

typedef struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
} Vertex;

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if(ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
		return true;

	PAINTSTRUCT paintStruct;
	HDC hDC;
	switch (message)
	{
	case WM_PAINT:
		hDC = BeginPaint(hwnd, &paintStruct);
		EndPaint(hwnd, &paintStruct);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow) {
	// Initialize and show window
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WindowCallback;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"DX11BookWindowClass";
	if (!RegisterClassEx(&wndClass))
		return -1;
	RECT rc = { 0, 0, screen_width, screen_height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hwnd = CreateWindowA("DX11BookWindowClass", "D3D11Prueba",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.
		left,
		rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!hwnd)
		return -1;
	ShowWindow(hwnd, cmdShow);
	
	// Initialize Direct3D 11
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Device* d3d_device;
	ID3D11DeviceContext* d3d_context;
	IDXGISwapChain* swap_chain;
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

	// Create the render target view and bind it to the context
	ID3D11RenderTargetView* render_target_view;
	ID3D11Texture2D* backbuffer;
	swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer));
	d3d_device->CreateRenderTargetView(backbuffer, 0, &render_target_view);
	backbuffer->Release();
	d3d_context->OMSetRenderTargets(1, &render_target_view, nullptr);

	// Create the viewport and bind it
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = screen_width;
	viewport.Height = screen_height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	d3d_context->RSSetViewports(1, &viewport);
	
	// Create vertex format description
	D3D11_INPUT_ELEMENT_DESC vertex_desc_buffer[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	// Create and bind vertex input layout and read vertex shader
	ID3D11InputLayout* input_layout;
	ID3DBlob* vertex_shader_blob;
	D3DReadFileToBlob(L"VertexShader.cso", &vertex_shader_blob);
	d3d_device->CreateInputLayout(vertex_desc_buffer, 2, vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &input_layout);
	d3d_context->IASetInputLayout(input_layout);

	// Actual vertices to render
	Vertex vertex_buffer_data[] = {
	{ DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f), Colors::Red },
	{ DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), Colors::Green },
	{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), Colors::Blue },
	{ DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f), Colors::Yellow },
	};
	// Create vertex buffer description
	D3D11_BUFFER_DESC vertex_buffer_desc;
	vertex_buffer_desc.ByteWidth = sizeof(vertex_buffer_data);
	vertex_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.CPUAccessFlags = 0;
	vertex_buffer_desc.MiscFlags = 0;
	vertex_buffer_desc.StructureByteStride = sizeof(Vertex);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	D3D11_SUBRESOURCE_DATA vertex_subresource_data;
	vertex_subresource_data.pSysMem = vertex_buffer_data;
	// Create hardware vertex buffer and bind it to the input assembler
	ID3D11Buffer* vertex_buffer;
	d3d_device->CreateBuffer(&vertex_buffer_desc, &vertex_subresource_data, &vertex_buffer);
	d3d_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	// Vertex indices buffer
	UINT vertex_indices[] = {
		0, 1, 2,
		0, 2, 3
	};
	// Create vertex indices buffer description
	D3D11_BUFFER_DESC vertex_indices_desc;
	vertex_indices_desc.ByteWidth = sizeof(vertex_indices);
	vertex_indices_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vertex_indices_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vertex_indices_desc.CPUAccessFlags = 0;
	vertex_indices_desc.MiscFlags = 0;
	vertex_indices_desc.StructureByteStride = sizeof(UINT);
	D3D11_SUBRESOURCE_DATA vertex_indices_subresource_data;
	vertex_indices_subresource_data.pSysMem = vertex_indices;
	// Create hardware vertex index buffer and bind it
	ID3D11Buffer* vertex_index_buffer;
	d3d_device->CreateBuffer(&vertex_indices_desc, &vertex_indices_subresource_data, &vertex_index_buffer);
	d3d_context->IASetIndexBuffer(vertex_index_buffer, DXGI_FORMAT_R32_UINT, 0);
	// Set primitive topology type to triangle list
	d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create and set vertex shader
	ID3D11VertexShader* vertex_shader;
	d3d_device->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), nullptr, &vertex_shader);
	d3d_context->VSSetShader(vertex_shader, nullptr, 0);

	// Create and set pixel shader
	ID3DBlob* pixel_shader_blob;
	D3DReadFileToBlob(L"PixelShader.cso", &pixel_shader_blob);
	ID3D11PixelShader* pixel_shader;
	d3d_device->CreatePixelShader(pixel_shader_blob->GetBufferPointer(), pixel_shader_blob->GetBufferSize(), nullptr, &pixel_shader);
	d3d_context->PSSetShader(pixel_shader, nullptr, 0);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(d3d_device, d3d_context);
	bool show_demo_window = true;

	const FLOAT clear_color[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
	// Event loop
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Update


		// Clear render target
		d3d_context->ClearRenderTargetView(render_target_view, clear_color);
		// Draw triangles from index buffer previously bound
		d3d_context->DrawIndexed(6, 0, 0);

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		// Render imgui widgets
		ImGui::ShowDemoWindow(&show_demo_window);
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Trigger a back buffer swap in the swap chain
		swap_chain->Present(1, 0);
	}

	// Cleanup
	// ImGui cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// Direct3D cleanup
	render_target_view->Release();
	swap_chain->Release();
	d3d_context->Release();
	d3d_device->Release();

	// Windows cleanup
	::DestroyWindow(hwnd);
	::UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);

	return static_cast<int>(msg.wParam);
}