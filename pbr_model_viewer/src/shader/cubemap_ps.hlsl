TextureCube cubemap_tex : register(t0);
SamplerState cubemap_sampler : register(s0);

float4 main(float4 pos : SV_POSITION, float3 world_pos : POSITION) : SV_Target
{
    float4 col = cubemap_tex.Sample(cubemap_sampler, world_pos);
    return col;
}