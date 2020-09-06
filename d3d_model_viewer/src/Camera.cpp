#include "Camera.h"

#include "Graphics.h"

Camera::Camera(DirectX::XMVECTOR position, DirectX::XMVECTOR lookat, DirectX::XMVECTOR right, DirectX::XMVECTOR up, float fov, float aspect_ratio)
	: position(position), up(up), right(right), lookat(lookat), fov(fov), aspect_ratio(aspect_ratio)
{
	persp_transf = DirectX::XMMatrixTranspose(
		DirectX::XMMatrixLookAtRH(position, lookat, up) *
		DirectX::XMMatrixPerspectiveFovRH(fov, aspect_ratio, 0.1f, 500.f)
	);
	persp_transf_buffer = Graphics::get()->create_buffer(&persp_transf, sizeof(DirectX::XMMATRIX), D3D11_BIND_CONSTANT_BUFFER);
	Graphics::get()->set_buffer(0, persp_transf_buffer);

	position_buffer = Graphics::get()->create_buffer(&position, sizeof(DirectX::XMVECTOR), D3D11_BIND_CONSTANT_BUFFER);
	Graphics::get()->set_buffer(1, position_buffer);
}

Camera::~Camera()
{
	persp_transf_buffer->Release();
}

void Camera::move(float delta_x, float delta_y, float delta_z)
{
	position.m128_f32[0] += delta_x;
	position.m128_f32[1] += delta_y;
	position.m128_f32[2] += delta_z;

	persp_transf = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtRH(position, lookat, up) * DirectX::XMMatrixPerspectiveFovRH(fov, aspect_ratio, 0.1f, 500.f));

	update_camera_shader_buffers();
}

void Camera::rotate(float angles_x, float angles_y)
{
	// Create rotation quaternion for x axis
	float angle_x_rad = DirectX::XMConvertToRadians(angles_x / 2.0f);
	DirectX::XMFLOAT3 quaternion_x_imaginary(sinf(angle_x_rad) * right.m128_f32[0], sinf(angle_x_rad) * right.m128_f32[1], sinf(angle_x_rad) * right.m128_f32[2]);
	float quaternion_x_real = cosf(angle_x_rad);
	DirectX::XMVECTOR quaternion_x = DirectX::XMVectorSet(quaternion_x_imaginary.x, quaternion_x_imaginary.y, quaternion_x_imaginary.z, quaternion_x_real);

	// Create rotation quaternion for y axis
	float angle_y_rad = DirectX::XMConvertToRadians(angles_y / 2.0f);
	DirectX::XMFLOAT3 quaternion_y_imaginary(0, sinf(angle_y_rad), 0);
	float quaternion_y_real = cosf(angle_y_rad);
	DirectX::XMVECTOR quaternion_y = DirectX::XMVectorSet(quaternion_y_imaginary.x, quaternion_y_imaginary.y, quaternion_y_imaginary.z, quaternion_y_real);

	// Combine quaternions
	DirectX::XMVECTOR quaternion = DirectX::XMQuaternionNormalize(DirectX::XMQuaternionMultiply(quaternion_x, quaternion_y));

	//Apply result quaternion to camera position and right vector
	{
		DirectX::XMVECTOR intermediate_result = DirectX::XMQuaternionMultiply(quaternion, DirectX::XMVectorSet(position.m128_f32[0], position.m128_f32[1], position.m128_f32[2], 0));
		intermediate_result = DirectX::XMQuaternionMultiply(intermediate_result, DirectX::XMQuaternionConjugate(quaternion));
		position.m128_f32[0] = intermediate_result.m128_f32[0];
		position.m128_f32[1] = intermediate_result.m128_f32[1];
		position.m128_f32[2] = intermediate_result.m128_f32[2];
	}
	{
		DirectX::XMVECTOR intermediate_result = DirectX::XMQuaternionMultiply(quaternion_y, DirectX::XMVectorSet(right.m128_f32[0], right.m128_f32[1], right.m128_f32[2], 0));
		intermediate_result = DirectX::XMQuaternionMultiply(intermediate_result, DirectX::XMQuaternionConjugate(quaternion_y));
		right.m128_f32[0] = intermediate_result.m128_f32[0];
		right.m128_f32[1] = intermediate_result.m128_f32[1];
		right.m128_f32[2] = intermediate_result.m128_f32[2];
	}

	// Update camera lookat and up vectors
	lookat = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMVectorSet(0, 0, 0, 2), position));
	lookat.m128_f32[3] = 1;
	up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(lookat, DirectX::XMVectorNegate(right)));
	up.m128_f32[3] = 1;

	persp_transf = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtRH(position, lookat, up) * DirectX::XMMatrixPerspectiveFovRH(fov, aspect_ratio, 0.1f, 500.f));

	update_camera_shader_buffers();
}

void Camera::update_camera_shader_buffers()
{
	Graphics::get()->update_buffer(persp_transf_buffer, &persp_transf, sizeof(DirectX::XMMATRIX));
	Graphics::get()->set_buffer(0, persp_transf_buffer);
	Graphics::get()->update_buffer(position_buffer, &position, sizeof(DirectX::XMVECTOR));
	Graphics::get()->set_buffer(1, position_buffer);
}
