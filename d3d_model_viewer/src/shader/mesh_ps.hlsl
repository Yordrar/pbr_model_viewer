static const float PI = 3.14159265359;

SamplerState tex_sampler : register(s0);
Texture2D albedo_tex : register(t0);
Texture2D normal_tex : register(t1);
Texture2D metallic_tex : register(t2);
Texture2D roughness_tex : register(t3);

float ggx(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float4 main(float4 pos : SV_POSITION, float3 cam_pos : POSITION0, float3 world_pos : POSITION1, float3 normal : NORMAL0, float2 uvs : TEXCOORDS, float3 tangent : TANGENT, float3 bitangent : BITANGENT) : SV_Target
{
    float2 UV = float2(uvs.x, 1.0 - uvs.y);
    float3 albedo = albedo_tex.Sample(tex_sampler, UV).rgb;
    albedo = pow(albedo, 2);
    float metallic = metallic_tex.Sample(tex_sampler, UV).r;
    float roughness = roughness_tex.Sample(tex_sampler, UV).r;
    
    float3 lightColor = float3(1.0, 1.0, 1.0);
    float3 L = normalize(float3(1.0, 1.0, 1.0));
    float3 V = normalize(cam_pos - world_pos);
    float3 H = normalize(L + V);
    float3 T = normalize(tangent);
    float3 B = normalize(cross(normal, tangent));
    float3 N = normalize(normal);
    float3x3 TBN = float3x3(T, B, N);
    float3 sampled_N = normal_tex.Sample(tex_sampler, UV).xyz;
    sampled_N = normalize(sampled_N * 2.0 - 1.0);
    N = normalize(mul(TBN, sampled_N));
    
    float NdotL = max(dot(N, L), 0.0);
    
    // Direct lighting
    float3 direct_light = lightColor;
    
    // Specular lighting
    float D = ggx(N, H, roughness);
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metallic);
    float HdotV = dot(H, V);
    float3 F = fresnelSchlick(max(HdotV, 0.0), F0);
    float G = GeometrySmith(N, V, L, roughness);
    
    float3 kS = F;
    float3 kD = float3(1.0, 1.0, 1.0) - kS;
    kD *= (1.0 - metallic);
    
    float3 numerator = D * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    float3 specular = numerator / max(denominator, 0.001);
    
    float3 col = (kD * albedo / PI + specular) * direct_light * NdotL;
    float3 ambient = float3(0.03, 0.03, 0.03) * albedo;
    col = ambient + col;
    col = col / (col + float3(1.0, 1.0, 1.0));
    col = sqrt(col);
    
    return float4(col, 1.0);
}