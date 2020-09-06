cbuffer perspective {
	matrix projection;
};

cbuffer camera_position {
	float4 cam_pos;
};

void main(float3 pos : POSITION, float4 col : COLOR, float3 normal : NORMAL,
			out float4 out_pos : SV_POSITION, out float4 out_col : COLOR) {

	out_pos = mul(float4(pos, 1.0f), projection);

	out_col.rgb = dot(normalize(cam_pos.xyz - pos), normalize(normal)) * float3(0.75f, 0.75f, 0.75f);
	out_col.a = 1.0f;
}