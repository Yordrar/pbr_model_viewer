cbuffer perspective {
	matrix projection;
};

cbuffer camera_position {
	float4 cam_pos;
};

void main(float3 pos : POSITION, float4 col : COLOR, float3 normal : NORMAL, float2 uvs : TEXCOORDS, float3 tangent : TANGENT, float3 bitangent : BITANGENT,
			out float4 out_pos : SV_POSITION, out float3 out_worldPos : POSITION) {

	out_pos = mul(float4(cam_pos.xyz + pos, 1.0f), projection);
	
    out_worldPos = pos;
}