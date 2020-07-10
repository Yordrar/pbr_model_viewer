cbuffer cbPerspective {
	matrix projection;
};

cbuffer camera_position {
	float4 cam_pos;
};

void main(float3 pos : POSITION, float4 col : COLOR, float3 normal : NORMAL,
			out float4 out_pos : SV_POSITION, out float4 out_col : COLOR) {

	out_pos = mul(float4(pos, 1.0f), projection);

	out_col = -dot(pos - cam_pos, normal) * float4(0.5f, 0.5f, 0.5f, 1.0f);
}