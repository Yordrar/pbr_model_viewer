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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Graphics.h"
#include "Camera.h"
#include "Vertex.h"
#include "Grid.h"
#include "Cubemap.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <drawable/IDrawable.h>
#include <bindable/IBindable.h>
#include <bindable/VertexBuffer.h>
#include <bindable/IndexBuffer.h>
#include <bindable/InputLayout.h>
#include <bindable/VertexShader.h>
#include <bindable/PixelShader.h>
#include <bindable/Texture.h>
#include <bindable/TextureSampler.h>
#include <bindable/TextureCube.h>

DirectX::XMFLOAT2 operator-(DirectX::XMFLOAT2 a, DirectX::XMFLOAT2 b)
{
	DirectX::XMFLOAT2 result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;

	return result;
}

DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
{
	DirectX::XMFLOAT3 result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;

	return result;
}

// Window parameters
int screen_width = 1280;
int screen_height = 720;

// Graphics
Graphics* gfx = nullptr;
Cubemap* cubemap = nullptr;
TextureCube* cubemap_texture = nullptr;

// Mesh
IDrawable* mesh = nullptr;
PixelShader* pbr_ps = nullptr;

// View options
bool show_wireframe = false;
bool show_grid = false;
bool show_cubemap = false;

// Loading popup
std::thread load_mesh_thread;
std::thread load_cubemap_thread;
bool show_loading_popup = false;

// Camera
Camera* cam;
DirectX::XMVECTOR camera_position = DirectX::XMVectorSet(0, 0, 5, 1);
DirectX::XMVECTOR camera_lookat_vector = DirectX::XMVectorSet(0, 0, -1, 1);
DirectX::XMVECTOR camera_right = DirectX::XMVectorSet(1, 0, 0, 1);
DirectX::XMVECTOR camera_up = DirectX::XMVectorSet(0, 1, 0, 1);
float near_plane = 0.1f;
float far_plane = 500.0f;


void load_obj_file(Graphics* gfx, std::string filename) {
	show_loading_popup = true;

	// Initialize mesh
	if ( mesh ) delete mesh;
	mesh = new IDrawable;
	VertexShader* mesh_vs_shader = new VertexShader( *gfx, "mesh_vs.cso" );
	mesh->addBindable( mesh_vs_shader );
	mesh->addBindable( new InputLayout( *gfx, vertex_desc_buffer, sizeof( vertex_desc_buffer ) / sizeof( D3D11_INPUT_ELEMENT_DESC ), mesh_vs_shader->getBytecode() ) );
	mesh->addBindable( new PixelShader( *gfx, "mesh_ps.cso" ) );
	mesh->addBindable( new TextureSampler( *gfx, 0, D3D11_FILTER_ANISOTROPIC ) );
	
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_ValidateDataStructure |
		aiProcess_GenUVCoords |
		aiProcess_FixInfacingNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	aiNode* rootNode = scene->mRootNode;
	aiMesh* ai_mesh = scene->mMeshes[rootNode->mChildren[0]->mMeshes[0]];

	// Destroy old vertex buffer if existed and create new with parsed vertices
	UINT vertices_count = ai_mesh->mNumVertices;
	Vertex* vertex_buffer_data = new Vertex[vertices_count];
	for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++)
	{
		Vertex vert;
		vert.position.x = ai_mesh->mVertices[i].x;
		vert.position.y = ai_mesh->mVertices[i].y;
		vert.position.z = ai_mesh->mVertices[i].z;

		vert.normal.x = ai_mesh->mNormals[i].x;
		vert.normal.y = ai_mesh->mNormals[i].y;
		vert.normal.z = ai_mesh->mNormals[i].z;

		vert.uvs.x = ai_mesh->mTextureCoords[0][i].x;
		vert.uvs.y = ai_mesh->mTextureCoords[0][i].y;

		vert.tangent.x = ai_mesh->mTangents[i].x;
		vert.tangent.y = ai_mesh->mTangents[i].y;
		vert.tangent.z = ai_mesh->mTangents[i].z;

		vert.bitangent.x = ai_mesh->mBitangents[i].x;
		vert.bitangent.y = ai_mesh->mBitangents[i].y;
		vert.bitangent.z = ai_mesh->mBitangents[i].z;

		vertex_buffer_data[i] = vert;
	}
	VertexBuffer* vertices = new VertexBuffer(*gfx, vertex_buffer_data, vertices_count);

	// Destroy old index buffer if existed and create new with parsed indices
	UINT indices_count = ai_mesh->mNumFaces * 3;
	UINT* vertex_indices_data = new UINT[indices_count];
	for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++)
	{
		aiFace face = ai_mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			vertex_indices_data[i * 3 + j] = face.mIndices[j];
	}
	IndexBuffer* indices = new IndexBuffer(*gfx, vertex_indices_data, indices_count);

	mesh->setMesh(vertices, indices);

	if ( cubemap_texture ) mesh->addBindable( cubemap_texture );

	show_loading_popup = false;
	ImGui::CloseCurrentPopup();
}


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool mouse_clicked = false;
int previous_pos_x = -1;
int previous_pos_y = -1;
LRESULT CALLBACK WindowCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if(ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam) ||
        ( gfx && ImGui::GetIO().WantCaptureMouse ))
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

		cam->move(0.0f, 0.0f, 0.0f);
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
	HWND hwnd = CreateWindowA("DX11WindowClass", "PBR Model Viewer",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.
		left,
		rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!hwnd)
		return -1;
	ShowWindow(hwnd, cmdShow);

	// Initialize graphics
	gfx = new Graphics(hwnd, screen_width, screen_height);
	pbr_ps = new PixelShader( *gfx, "mesh_pbr_ps.cso" );

	// Camera
	cam = new Camera(*gfx, camera_position, camera_lookat_vector, camera_right, camera_up, DirectX::XM_PI / 4.0f, float(screen_width) / float(screen_height));

	// Create grid
	Grid grid;
	
	// Initialize initial camera position and orientation
	cam->rotate(30, 0);
	cam->rotate(0, -45);

	const FLOAT clear_color_black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const FLOAT clear_color_grey[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	// Event loop
	MSG msg = { 0 };
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Draw if not loading any mesh
		if (!show_loading_popup) {

			gfx->clear(clear_color_black);

			if (show_grid)
			{
				grid.draw();
			}

			if (cubemap && show_cubemap)
			{
				cubemap->draw(*gfx);
			}

			// Set wireframe or solid mode according to view options
			if (show_wireframe) {
				gfx->change_fill_mode(D3D11_FILL_WIREFRAME);
			}
			else {
				gfx->change_fill_mode(D3D11_FILL_SOLID);
			}

			if( mesh ) mesh->draw(*gfx);
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
				if ( ImGui::MenuItem( "Open Mesh File..." ) )
				{
					ImGuiFileDialog::Instance()->OpenDialog( "open_mesh_dialog", "Choose mesh file", "Mesh file(*.obj *.fbx){.obj,.fbx}", "." );
				}
				if ( ImGui::MenuItem( "Open Cubemap Folder..." ) )
				{
					ImGuiFileDialog::Instance()->OpenDialog( "open_cubemap_dialog", "Choose cubemap folder", nullptr, "." );
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Wireframe", nullptr, &show_wireframe);
				ImGui::MenuItem("Grid", nullptr, &show_grid);
				ImGui::MenuItem("Cubemap", nullptr, &show_cubemap);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		if ( mesh && ImGui::Begin( "PBR maps", nullptr, ImGuiWindowFlags_None ) )
		{
			if ( ImGui::Button( "Load albedo texture" ) )
			{
				ImGuiFileDialog::Instance()->OpenDialog( "open_albedo_dialog", "Choose albedo texture", "Image files (*.jpeg/jpg *.png *.tga *.bmp){.jpeg,.jpg,.png,.tga,.bmp}", "." );
			}
			if ( ImGui::Button( "Load normal map" ) )
			{
				ImGuiFileDialog::Instance()->OpenDialog( "open_normal_dialog", "Choose normal map", "Image files (*.jpeg/jpg *.png *.tga *.bmp){.jpeg,.jpg,.png,.tga,.bmp}", "." );
			}
			if ( ImGui::Button( "Load metallic map" ) )
			{
				ImGuiFileDialog::Instance()->OpenDialog( "open_metallic_dialog", "Choose metallic map", "Image files (*.jpeg/jpg *.png *.tga *.bmp){.jpeg,.jpg,.png,.tga,.bmp}", "." );
			}
            if ( ImGui::Button( "Load roughness map" ) )
            {
                ImGuiFileDialog::Instance()->OpenDialog( "open_roughness_dialog", "Choose roughness map", "Image files (*.jpeg/jpg *.png *.tga *.bmp){.jpeg,.jpg,.png,.tga,.bmp}", "." );
            }
			ImGui::End();
		}
		if (ImGuiFileDialog::Instance()->Display("open_mesh_dialog")) {
			if (ImGuiFileDialog::Instance()->IsOk()) {
				std::string filename = ImGuiFileDialog::Instance()->GetFilePathName();
				if (load_mesh_thread.joinable()) load_mesh_thread.join();
				load_mesh_thread = std::thread(load_obj_file, gfx, filename);
				ImGui::OpenPopup("Loading...", 0);
			}
			ImGuiFileDialog::Instance()->Close();
		}
		if ( ImGuiFileDialog::Instance()->Display( "open_albedo_dialog" ) )
		{
			if ( ImGuiFileDialog::Instance()->IsOk() )
			{
				mesh->addBindable( new Texture( *gfx, ImGuiFileDialog::Instance()->GetFilePathName(), 0 ) );
				mesh->changePixelShader( pbr_ps );
			}
			ImGuiFileDialog::Instance()->Close();
		}
		if ( ImGuiFileDialog::Instance()->Display( "open_normal_dialog" ) )
		{
			if ( ImGuiFileDialog::Instance()->IsOk() )
			{
				mesh->addBindable( new Texture( *gfx, ImGuiFileDialog::Instance()->GetFilePathName(), 1 ) );
				mesh->changePixelShader( pbr_ps );
			}
			ImGuiFileDialog::Instance()->Close();
		}
		if ( ImGuiFileDialog::Instance()->Display( "open_metallic_dialog" ) )
		{
			if ( ImGuiFileDialog::Instance()->IsOk() )
			{
				mesh->addBindable( new Texture( *gfx, ImGuiFileDialog::Instance()->GetFilePathName(), 2 ) );
				mesh->changePixelShader( pbr_ps );
			}
			ImGuiFileDialog::Instance()->Close();
		}
		if ( ImGuiFileDialog::Instance()->Display( "open_roughness_dialog" ) )
		{
			if ( ImGuiFileDialog::Instance()->IsOk() )
			{
				mesh->addBindable( new Texture( *gfx, ImGuiFileDialog::Instance()->GetFilePathName(), 3 ) );
				mesh->changePixelShader( pbr_ps );
			}
			ImGuiFileDialog::Instance()->Close();
		}
		if ( ImGuiFileDialog::Instance()->Display( "open_cubemap_dialog" ) )
		{
			if ( ImGuiFileDialog::Instance()->IsOk() )
			{
				if ( cubemap ) delete cubemap;
				cubemap = new Cubemap( *gfx, ImGuiFileDialog::Instance()->GetCurrentPath() );
				if ( mesh ) mesh->deleteBindable( cubemap_texture );
				if ( cubemap_texture ) delete cubemap_texture;
				cubemap_texture = new TextureCube( *gfx, ImGuiFileDialog::Instance()->GetCurrentPath(), 4 );
				if ( mesh ) mesh->addBindable( cubemap_texture );
				show_cubemap = true;
			}
			ImGuiFileDialog::Instance()->Close();
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
		gfx->present();
	}

	// Cleanup
	// Loading thread cleanup
	if (load_mesh_thread.joinable()) load_mesh_thread.join();

	// Graphics cleanup
	if ( mesh ) delete mesh;
	if ( cubemap ) delete cubemap;
	delete gfx;

	// Windows cleanup
	::DestroyWindow(hwnd);
	::UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);

	return static_cast<int>(msg.wParam);
}