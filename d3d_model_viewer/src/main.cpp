#include <Windows.h>

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#include "imgui.h"
#include "ImGuiFileDialog.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <string>
#include <iostream>
#include <fstream>

namespace Colors {
	XMGLOBALCONST DirectX::XMFLOAT4 White = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::XMFLOAT4 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMFLOAT4 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMFLOAT4 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMFLOAT4 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::XMFLOAT4 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMFLOAT4 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::XMFLOAT4 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };
}

typedef struct Vertex {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT3 normal;
} Vertex;

// Window parameters
int screen_width = 1280;
int screen_height = 720;

// Direct3D device and context
ID3D11Device* d3d_device;
ID3D11DeviceContext* d3d_context;

// Vertex buffer, its buffer description and its subresource data
Vertex* vertex_buffer_data = nullptr;
int vertices_count = 0;
D3D11_BUFFER_DESC vertex_buffer_desc;
D3D11_SUBRESOURCE_DATA vertex_subresource_data;
ID3D11Buffer* vertex_buffer = nullptr;

// Vertex indices buffer, its buffer description and its subresource data
UINT* vertex_indices_data = nullptr;
int indices_count = 0;
D3D11_BUFFER_DESC vertex_indices_desc;
D3D11_SUBRESOURCE_DATA vertex_indices_subresource_data;
ID3D11Buffer* vertex_index_buffer = nullptr;

// Camera
DirectX::XMVECTOR camera_position = DirectX::XMVectorSet(0, 0, 0, 1);

std::vector<std::string> split(std::string& str, char pattern) {
	int pos_init = 0;
	int pos_found = 0;
	std::string splitted;
	std::vector<std::string> result;

	while (pos_found >= 0) {
		pos_found = str.find(pattern, pos_init);
		splitted = str.substr(pos_init, pos_found - pos_init);
		pos_init = pos_found + 1;
		result.push_back(splitted);
	}

	return result;
}
void load_obj_file(std::string filename) {
	std::vector<Vertex> parsed_vertices;
	std::vector<UINT> parsed_indices;
	std::vector<DirectX::XMFLOAT3> parsed_normals;

	std::ifstream file(filename, std::ifstream::in);
	std::string line;
	while (std::getline(file, line)) {
		if (line.find("o ", 0) == 0) {
			std::string object_name = line.substr(2);
			std::cout << "Object: " << object_name << std::endl;
		}
		else if (line.find("v ", 0) == 0) {
			std::vector<std::string> vertices_str = split(line, ' ');
			Vertex v;
			v.position.x = std::stof(vertices_str[1]);
			v.position.y = std::stof(vertices_str[2]);
			v.position.z = std::stof(vertices_str[3]);
			int random_num = (int)(rand() % 6);
			switch (random_num) {
			case 0:
				v.color = Colors::Blue;
				break;
			case 1:
				v.color = Colors::Cyan;
				break;
			case 2:
				v.color = Colors::Green;
				break;
			case 3:
				v.color = Colors::Magenta;
				break;
			case 4:
				v.color = Colors::Red;
				break;
			case 5:
				v.color = Colors::Yellow;
				break;
			default:
				v.color = Colors::Black;
				break;
			}
			parsed_vertices.push_back(v);
		}
		else if (line.find("f ", 0) == 0) {
			// Read vertex indices
			std::vector<std::string> face_str = split(line, ' ');
			UINT face_vertex1 = std::stoi(split(face_str[1], '/')[0])-1;
			UINT face_vertex2 = std::stoi(split(face_str[2], '/')[0])-1;
			UINT face_vertex3 = std::stoi(split(face_str[3], '/')[0])-1;
			parsed_indices.push_back(face_vertex1);
			parsed_indices.push_back(face_vertex2);
			parsed_indices.push_back(face_vertex3);

			// Read normal indices and assign normals
			float normal1 = std::stoi(split(face_str[1], '/')[2])-1;
			float normal2 = std::stoi(split(face_str[2], '/')[2])-1;
			float normal3 = std::stoi(split(face_str[3], '/')[2])-1;
			parsed_vertices[face_vertex1].normal = parsed_normals[normal1];
			parsed_vertices[face_vertex2].normal = parsed_normals[normal2];
			parsed_vertices[face_vertex3].normal = parsed_normals[normal3];
		}
		else if (line.find("vn ", 0) == 0) {
			std::vector<std::string> normals_str = split(line, ' ');
			DirectX::XMFLOAT3 n;
			n.x = std::stof(normals_str[1]);
			n.y = std::stof(normals_str[2]);
			n.z = std::stof(normals_str[3]);
			parsed_normals.push_back(n);
		}
	}
	// Destroy old vertex buffer if existed and create new with parsed vertices
	vertices_count = parsed_vertices.size();
	if (vertex_buffer_data) delete[] vertex_buffer_data;
	vertex_buffer_data = new Vertex[vertices_count];
	std::copy(parsed_vertices.begin(), parsed_vertices.end(), vertex_buffer_data);

	// Destroy old index buffer if existed and create new with parsed indices
	indices_count = parsed_indices.size();
	if (vertex_indices_data) delete[] vertex_indices_data;
	vertex_indices_data = new UINT[indices_count];
	std::copy(parsed_indices.begin(), parsed_indices.end(), vertex_indices_data);

	// Create vertex buffer description
	vertex_buffer_desc.ByteWidth = vertices_count * sizeof(Vertex);
	vertex_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.CPUAccessFlags = 0;
	vertex_buffer_desc.MiscFlags = 0;
	vertex_buffer_desc.StructureByteStride = sizeof(Vertex);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	vertex_subresource_data.pSysMem = vertex_buffer_data;
	// Create hardware vertex buffer and bind it to the input assembler
	if (vertex_buffer) vertex_buffer->Release();
	d3d_device->CreateBuffer(&vertex_buffer_desc, &vertex_subresource_data, &vertex_buffer);
	d3d_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);


	// Create vertex indices buffer description
	vertex_indices_desc.ByteWidth = indices_count * sizeof(UINT);
	vertex_indices_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vertex_indices_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vertex_indices_desc.CPUAccessFlags = 0;
	vertex_indices_desc.MiscFlags = 0;
	vertex_indices_desc.StructureByteStride = sizeof(UINT);
	vertex_indices_subresource_data.pSysMem = vertex_indices_data;
	// Create hardware vertex index buffer and bind it to the input assembler
	if (vertex_index_buffer) vertex_index_buffer->Release();
	d3d_device->CreateBuffer(&vertex_indices_desc, &vertex_indices_subresource_data, &vertex_index_buffer);
	d3d_context->IASetIndexBuffer(vertex_index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// Set primitive topology type to triangle list
	d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


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
	HWND hwnd = CreateWindowA("DX11BookWindowClass", "D3D Model Viewer",
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

	// Create depth and stencil buffers
	ID3D11Texture2D* depth_stencil_buffer;
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
	ID3D11DepthStencilView* depth_stencil_view;
	d3d_device->CreateDepthStencilView(depth_stencil_buffer, nullptr, &depth_stencil_view);

	// Create the render target view
	ID3D11RenderTargetView* render_target_view;
	ID3D11Texture2D* backbuffer;
	swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer));
	d3d_device->CreateRenderTargetView(backbuffer, 0, &render_target_view);
	backbuffer->Release();

	// Bind the render target and depth/stencil buffer to the output merger
	d3d_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

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
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	// Create and bind vertex input layout and read vertex shader
	ID3D11InputLayout* input_layout;
	ID3DBlob* vertex_shader_blob;
	D3DReadFileToBlob(L"VertexShader.cso", &vertex_shader_blob);
	d3d_device->CreateInputLayout(vertex_desc_buffer, sizeof(vertex_desc_buffer)/sizeof(D3D11_INPUT_ELEMENT_DESC), vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &input_layout);
	d3d_context->IASetInputLayout(input_layout);

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

	// Create perspective transform and bind ti to the vertex shader
	DirectX::XMMATRIX persp_transf;
	persp_transf = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtRH(camera_position,
																		DirectX::XMVectorSet(0, 0, -1, 0),
																		DirectX::XMVectorSet(0, 1, 0, 0)) * DirectX::XMMatrixPerspectiveFovRH(DirectX::XM_PI / 4.0f, float(screen_width) / float(screen_height), 0.1f, 100.0f));
	D3D11_BUFFER_DESC transform_desc;
	transform_desc.ByteWidth = sizeof(DirectX::XMMATRIX);
	transform_desc.Usage = D3D11_USAGE_DYNAMIC;
	transform_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	transform_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	transform_desc.MiscFlags = 0;
	transform_desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA transform_subres_data;
	transform_subres_data.pSysMem = &persp_transf;
	ID3D11Buffer* transform_buffer;
	d3d_device->CreateBuffer(&transform_desc, &transform_subres_data, &transform_buffer);
	d3d_context->VSSetConstantBuffers(0, 1, &transform_buffer);

	// Create buffer with camera position
	D3D11_BUFFER_DESC cam_pos_desc;
	cam_pos_desc.ByteWidth = sizeof(DirectX::XMVECTOR);
	cam_pos_desc.Usage = D3D11_USAGE_DYNAMIC;
	cam_pos_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cam_pos_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cam_pos_desc.MiscFlags = 0;
	cam_pos_desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA cam_pos_subres_data;
	cam_pos_subres_data.pSysMem = &camera_position;
	ID3D11Buffer* cam_pos_buffer;
	d3d_device->CreateBuffer(&cam_pos_desc, &cam_pos_subres_data, &cam_pos_buffer);
	d3d_context->VSSetConstantBuffers(1, 1, &cam_pos_buffer);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(d3d_device, d3d_context);
	bool show_demo_window = true;

	const FLOAT clear_color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	// Event loop
	MSG msg = { 0 };
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Update


		// Clear render target
		d3d_context->ClearRenderTargetView(render_target_view, clear_color);
		// Clear depth buffer
		d3d_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
		// Draw triangles from index buffer previously bound
		if(vertex_buffer_data && vertex_indices_data)
			d3d_context->DrawIndexed(indices_count, 0, 0);

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		// Render imgui widgets
		//ImGui::ShowDemoWindow(&show_demo_window);
		if(ImGui::BeginMainMenuBar())
		{
			if(ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open File...")) {
					igfd::ImGuiFileDialog::Instance()->OpenDialog("open_dialog", "Choose mesh file", ".obj", ".");
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		if (igfd::ImGuiFileDialog::Instance()->FileDialog("open_dialog")) {
			if (igfd::ImGuiFileDialog::Instance()->IsOk) {
				std::string filename = igfd::ImGuiFileDialog::Instance()->GetFilepathName();
				load_obj_file(filename);
			}
			igfd::ImGuiFileDialog::Instance()->CloseDialog("open_dialog");
		}
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