float4 main(float4 pos : SV_POSITION, float3 cam_pos : POSITION0, float3 world_pos : POSITION1, float3 normal : NORMAL0, float2 uvs : TEXCOORDS, float3 tangent : TANGENT, float3 bitangent : BITANGENT) : SV_Target
{
    float4 col = float4(0.5f, 0.5f, 0.5f, 1.0f) * max(dot(normalize(cam_pos - world_pos), normalize(normal)), 0.0f);
    return sqrt(col);
}