#pragma once

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

class Camera
{
public:
	Camera(DirectX::XMVECTOR position, DirectX::XMVECTOR lookat, DirectX::XMVECTOR right, DirectX::XMVECTOR up, float fov, float aspect_ratio);
	~Camera();

	void move(float delta_x, float delta_y, float delta_z);
	void rotate(float angles_x, float angles_y);

	void update_camera_shader_buffers();

	DirectX::XMMATRIX persp_transf;
	ID3D11Buffer* persp_transf_buffer;
	DirectX::XMVECTOR position;
	ID3D11Buffer* position_buffer;
	DirectX::XMVECTOR right;
	DirectX::XMVECTOR up;
	DirectX::XMVECTOR lookat;
	float aspect_ratio;
	float fov;
};

