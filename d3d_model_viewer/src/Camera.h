#pragma once

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#include <bindable/ConstantBuffer.h>
#include <Graphics.h>

class Camera
{
public:
	Camera(Graphics& gfx, DirectX::XMVECTOR position, DirectX::XMVECTOR lookat, DirectX::XMVECTOR right, DirectX::XMVECTOR up, float fov, float aspect_ratio);
	~Camera();

	void move(float delta_x, float delta_y, float delta_z);
	void rotate(float angles_x, float angles_y);

	void update_camera_shader_buffers();

	Graphics& m_graphics;

	DirectX::XMMATRIX m_viewProjMatrix;
	ConstantBuffer* m_viewProjBuffer;
	DirectX::XMVECTOR position;
	ConstantBuffer* m_positionBuffer;

	DirectX::XMVECTOR right;
	DirectX::XMVECTOR up;
	DirectX::XMVECTOR lookat;
	float aspect_ratio;
	float fov;
};

