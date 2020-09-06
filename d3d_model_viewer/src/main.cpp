#include <Windows.h>
#include <Windowsx.h>

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#include <string>
#include <iostream>
#include <fstream>
#include <thread>

#include "imgui/imgui.h"
#include "imgui/ImGuiFileDialog.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "Graphics.h"
#include "Camera.h"
#include "Vertex.h"
#include "Grid.h"

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

// Window parameters
int screen_width = 1280;
int screen_height = 720;

// Vertex buffer, its buffer description and its subresource data
Vertex* vertex_buffer_data = nullptr;
int vertices_count = 0;
D3D11_BUFFER_DESC vertex_buffer_desc;
D3D11_SUBRESOURCE_DATA vertex_subresource_data;
ID3D11Buffer* vertex_buffer = nullptr;
UINT stride = sizeof(Vertex);
UINT offset = 0;

// Vertex indices buffer, its buffer description and its subresource data
UINT* vertex_indices_data = nullptr;
int indices_count = 0;
D3D11_BUFFER_DESC vertex_indices_desc;
D3D11_SUBRESOURCE_DATA vertex_indices_subresource_data;
ID3D11Buffer* vertex_index_buffer = nullptr;

// Vertex normals buffer, its buffer description and its subresource data
Vertex* vertex_normal_data = nullptr;
int normal_count = 0;
D3D11_BUFFER_DESC vertex_normal_desc;
D3D11_SUBRESOURCE_DATA vertex_normal_subresource_data;
ID3D11Buffer* vertex_normal_buffer = nullptr;

// View options
bool show_wireframe = false;
bool show_normals = false;
bool smooth_shading = false;

// Loading popup
std::thread load_thread;
bool show_loading_popup = false;

// Camera
Camera* cam;
DirectX::XMVECTOR camera_position = DirectX::XMVectorSet(0, 0, 5, 1);
DirectX::XMVECTOR camera_lookat_vector = DirectX::XMVectorSet(0, 0, -1, 1);
DirectX::XMVECTOR camera_right = DirectX::XMVectorSet(1, 0, 0, 1);
DirectX::XMVECTOR camera_up = DirectX::XMVectorSet(0, 1, 0, 1);
float near_plane = 0.1f;
float far_plane = 500.0f;

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
	show_loading_popup = true;
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
			int normal1 = std::stoi(split(face_str[1], '/')[2])-1;
			int normal2 = std::stoi(split(face_str[2], '/')[2])-1;
			int normal3 = std::stoi(split(face_str[3], '/')[2])-1;
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

	// Destroy old normal buffer if existed and create new with parsed normals
	normal_count = parsed_normals.size();
	if (vertex_normal_data) delete[] vertex_normal_data;
	vertex_normal_data = new Vertex[vertices_count * 2];
	int j = 0;
	for (int i = 0; i < vertices_count * 2; i += 2) {
		vertex_normal_data[i] = vertex_buffer_data[j];
		vertex_normal_data[i + 1] = vertex_buffer_data[j];
		vertex_normal_data[i + 1].position.x += vertex_buffer_data[j].normal.x / 10.0f;
		vertex_normal_data[i + 1].position.y += vertex_buffer_data[j].normal.y / 10.0f;
		vertex_normal_data[i + 1].position.z += vertex_buffer_data[j].normal.z / 10.0f;
		j++;
	}

	// Create vertex buffer description
	vertex_buffer_desc.ByteWidth = vertices_count * sizeof(Vertex);
	vertex_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.CPUAccessFlags = 0;
	vertex_buffer_desc.MiscFlags = 0;
	vertex_buffer_desc.StructureByteStride = sizeof(Vertex);
	vertex_subresource_data.pSysMem = vertex_buffer_data;
	// Create hardware vertex buffer and bind it to the input assembler
	if (vertex_buffer) vertex_buffer->Release();
	Graphics::get()->d3d_device->CreateBuffer(&vertex_buffer_desc, &vertex_subresource_data, &vertex_buffer);


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
	Graphics::get()->d3d_device->CreateBuffer(&vertex_indices_desc, &vertex_indices_subresource_data, &vertex_index_buffer);


	// Create normal buffer description
	vertex_normal_desc.ByteWidth = vertices_count * 2 * sizeof(Vertex);
	vertex_normal_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vertex_normal_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_normal_desc.CPUAccessFlags = 0;
	vertex_normal_desc.MiscFlags = 0;
	vertex_normal_desc.StructureByteStride = sizeof(Vertex);
	vertex_normal_subresource_data.pSysMem = vertex_normal_data;
	// Create hardware normal buffer and bind it to the input assembler
	if (vertex_normal_buffer) vertex_normal_buffer->Release();
	Graphics::get()->d3d_device->CreateBuffer(&vertex_normal_desc, &vertex_normal_subresource_data, &vertex_normal_buffer);

	show_loading_popup = false;
	ImGui::CloseCurrentPopup();
}


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool mouse_clicked = false;
int previous_pos_x = -1;
int previous_pos_y = -1;
LRESULT CALLBACK WindowCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if(ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
		return true;

	PAINTSTRUCT paintStruct;
	HDC hDC;
	switch (message) {
	case WM_PAINT:
		hDC = BeginPaint(hwnd, &paintStruct);
		EndPaint(hwnd, &paintStruct);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		mouse_clicked = true;
		previous_pos_x = GET_X_LPARAM(lParam);
		previous_pos_y = GET_Y_LPARAM(lParam);
		break;
	case WM_LBUTTONUP:
		mouse_clicked = false;
		previous_pos_x = -1;
		previous_pos_y = -1;
		break;
	case WM_MOUSEMOVE:
		if (mouse_clicked) {
			int pos_x = GET_X_LPARAM(lParam);
			int pos_y = GET_Y_LPARAM(lParam);

			cam->rotate((pos_y - previous_pos_y) / 3.5f, 0);
			cam->rotate(0, (pos_x - previous_pos_x) / 3.5f);

			previous_pos_x = pos_x;
			previous_pos_y = pos_y;
		}
		break;
	case WM_MOUSEWHEEL:
	{
		int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		cam->position.m128_f32[0] *= 1.0f - (delta / 500.0f);
		cam->position.m128_f32[1] *= 1.0f - (delta / 500.0f);
		cam->position.m128_f32[2] *= 1.0f - (delta / 500.0f);

		cam->update_camera_shader_buffers();
		break;
	}
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
	wndClass.lpszClassName = L"DX11WindowClass";
	if (!RegisterClassEx(&wndClass))
		return -1;
	RECT rc = { 0, 0, screen_width, screen_height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hwnd = CreateWindowA("DX11WindowClass", "D3D Model Viewer",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.
		left,
		rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!hwnd)
		return -1;
	ShowWindow(hwnd, cmdShow);

	// Initialize graphics
	Graphics::create(hwnd, screen_width, screen_height);

	// Camera
	cam = new Camera(camera_position, camera_lookat_vector, camera_right, camera_up, DirectX::XM_PI / 4.0f, float(screen_width) / float(screen_height));

	// Create grid
	Grid grid;

	// Initialize initial camera position and orientation
	cam->rotate(30, 0);
	cam->rotate(0, -45);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(Graphics::get()->d3d_device, Graphics::get()->d3d_context);

	const FLOAT clear_color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	// Event loop
	MSG msg = { 0 };
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Graphics::get()->clear(clear_color);

		grid.draw();

		// Draw mesh if any has been read
		if (vertex_buffer_data && vertex_indices_data && vertex_normal_data) {
			if (show_normals) {
				// Set vertex buffer to the normals buffer
				Graphics::get()->d3d_context->IASetVertexBuffers(0, 1, &vertex_normal_buffer, &stride, &offset);
				// Set the shader to the normals shader
				Graphics::get()->d3d_context->VSSetShader(Graphics::get()->vertex_normal_shader, nullptr, 0);
				// Set primitive topology type to line list
				Graphics::get()->d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
				//Draw normals
				Graphics::get()->d3d_context->Draw(normal_count * 2, 0);
			}

			// Set wireframe or solid mode according to view options
			if (show_wireframe) {
				Graphics::get()->change_fill_mode(D3D11_FILL_WIREFRAME);
			}
			else {
				Graphics::get()->change_fill_mode(D3D11_FILL_SOLID);
			}

			// Set vertex and index buffer of mesh
			Graphics::get()->d3d_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
			Graphics::get()->d3d_context->VSSetShader(Graphics::get()->vertex_shader, nullptr, 0);
			Graphics::get()->d3d_context->IASetIndexBuffer(vertex_index_buffer, DXGI_FORMAT_R32_UINT, 0);
			// Set primitive topology type to triangle list
			Graphics::get()->d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//Draw mesh
			Graphics::get()->d3d_context->DrawIndexed(indices_count, 0, 0);
		}

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
			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Wireframe", nullptr, &show_wireframe);
				ImGui::MenuItem("View normals", nullptr, &show_normals);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		if (igfd::ImGuiFileDialog::Instance()->FileDialog("open_dialog")) {
			if (igfd::ImGuiFileDialog::Instance()->IsOk) {
				std::string filename = igfd::ImGuiFileDialog::Instance()->GetFilepathName();
				if (load_thread.joinable()) load_thread.join();
				load_thread = std::thread(load_obj_file, filename);
				ImGui::OpenPopup("Loading...", 0);
			}
			igfd::ImGuiFileDialog::Instance()->CloseDialog("open_dialog");
		}
		if (show_loading_popup) {
			ImGui::SetNextWindowSize(ImVec2(160, 50));
			if (ImGui::BeginPopupModal("Loading...", nullptr, ImGuiWindowFlags_NoMove |
															  ImGuiWindowFlags_NoResize |
															  ImGuiWindowFlags_NoScrollbar |
															  ImGuiWindowFlags_NoScrollWithMouse)) {
				ImGui::LabelText("", "Loading mesh...");
				ImGui::EndPopup();
			}
		}
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Trigger a back buffer swap in the swap chain
		Graphics::get()->present();
	}

	// Cleanup
	// Loading thread cleanup
	if (load_thread.joinable()) load_thread.join();
	// ImGui cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Graphics::destroy();

	// Windows cleanup
	::DestroyWindow(hwnd);
	::UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);

	return static_cast<int>(msg.wParam);
}