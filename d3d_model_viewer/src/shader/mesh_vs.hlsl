cbuffer perspective {
	matrix projection;
};

cbuffer camera_position {
	float4 cam_pos;
};

void main(float3 pos : POSITION, float4 col : COLOR, float3 normal : NORMAL, float2 uvs : TEXCOORDS, float3 tangent : TANGENT,
			out float4 out_pos : SV_POSITION, out float3 out_cam_pos : POSITION0, out float3 out_world_pos : POSITION1, out float3 out_normal : NORMAL0, out float2 out_uvs : TEXCOORDS, out float3 out_tangent : TANGENT)
{
	out_pos = mul(float4(pos, 1.0f), projection);
    out_cam_pos = cam_pos;
    out_world_pos = pos;
    out_normal = normal;
    out_uvs = uvs;
    out_tangent = tangent;
}