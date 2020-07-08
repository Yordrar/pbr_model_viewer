cbuffer cbPerspective
{
	matrix projection;
}; 

void main( float3 pos : POSITION, float4 col : COLOR,
			out float4 out_pos : SV_POSITION, out float4 out_col : COLOR)
{
	out_pos = mul(float4(pos, 1.0f), projection);

	out_col = col;
}