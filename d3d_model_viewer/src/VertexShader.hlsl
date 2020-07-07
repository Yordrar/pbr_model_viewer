cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
}; 

void main( float3 pos : POSITION, float4 col : COLOR,
			out float4 out_pos : SV_POSITION, out float4 out_col : COLOR)
{
	out_pos = float4(pos.x, pos.y, pos.z, 1.0f);

	out_col = col;
}