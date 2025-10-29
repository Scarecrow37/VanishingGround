#ifndef __FUNCTION__
#define __FUNCTION__

#include "CommonData.hlsli"

inline float3 GammaToLinearSpace(float3 sRGB)
{
    return sRGB * (sRGB * (sRGB * 0.305306011 + 0.682171111) + 0.012522878);
}

inline float3 LinearToGammaSpace(float3 linRGB)
{
    linRGB = max(linRGB, float3(0.0, 0.0, 0.0));
    return max(1.055 * pow(linRGB, 0.416666667) - 0.055, 0.0);
}

inline float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.f - F0) * pow(1.f - cosTheta, 5);
}

inline float NormalDistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = saturate(dot(N, H));
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.f) + 1.f);
    
    return a2 / (PI * denom * denom + Epsilon);
}

inline float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1;
    float k = (r * r) / 8.f;
    
    return NdotV / (NdotV * (1.f - k) + k + Epsilon);
}

inline float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = saturate(dot(N, V));
    float NdotL = saturate(dot(N, L));
    
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

inline float3 DiffuseBRDF(float3 N, float3 V, float3 L, float3 albedo, float metallic, float roughness)
{
    float3 H = normalize(V + L);
    float  NdotL = saturate(dot(N, L));
    float  VdotH = saturate(dot(V, H));
    
    float3 F0 = lerp(float3(Fdielectric, Fdielectric, Fdielectric), albedo, metallic);
    float3 F = FresnelSchlick(VdotH, F0);
    float3 D = NormalDistributionGGX(N, H, roughness);
    float3 G = GeometrySmith(N, V, L, roughness);
    
    float3 specular = (D * G * F) / max(4.0 * saturate(dot(N, V)) * NdotL, Epsilon);
    float3 kd = (1.0 - F) * (1.0 - metallic);
    float3 diffuse = kd * albedo / PI;

    return (diffuse + specular) * NdotL;
}

inline float Attenuation(float3 attenuation, float distance, float range)
{
    //float result = 1.f / (attenuation.r + attenuation.g * distance + attenuation.b * distance * distance);
    //result *= 2;
    
    float result = saturate(1.0f - (distance * distance / (range * range)));
    result = result * result;

    //// 사이거리 - 광원의 반지름을 구하여 진짜 거리 계산
    //float d = max(distance - range, 0); // 거리가 음수면 0으로 설정

    //// 감쇠 계산을 위한 중간 값 설정 (거리 / 광원의 반지름 + 1)
    //float denom = (d / range) + 1;

    //// 감쇠 강도 계산 (거리가 크면 감쇠되고 반지름이 크면 빛을 많이 받음)
    //float result = 1 / (denom * denom);

    //// 감쇠 스케일 재조정 임계값
    //// att가 0일때는 광원과 가장 멈
    //// att가 1일 때는 광원 중심에 가장 가까움
    //result = (result - Epsilon) / (1 - Epsilon);


    //// att가 음수가 나오지 않도록 조정
    //result = saturate(result);
    
    return result;
}

inline float3 CalculateDirectional(DirectionalLight light, float3 N, float3 V, float3 albedo, float metallic, float roughness)
{
    float3 L = -light.Direction;    
    return DiffuseBRDF(N, V, L, albedo, metallic, roughness) * light.Color * light.Intensity;
}

inline float3 CalculatePoint(PointLight light, float3 N, float3 V, float3 albedo, float metallic, float roughness, float3 fragPos)
{
    float3 L = light.Position - fragPos;
    float distance = length(L);
    L = normalize(L);    
    float attenuation = Attenuation(light.Attenuation, distance, light.Range);
        
    return DiffuseBRDF(N, V, L, albedo, metallic, roughness) * attenuation * light.Color * light.Intensity;
}

inline float3 CalculateSpot(SpotLight light, float3 N, float3 V, float3 albedo, float metallic, float roughness, float3 fragPos)
{
    float3 L = light.Position - fragPos;
    float distance = length(L);
    L /= distance;
    float theta = dot(-L, normalize(light.Direction));

    float epsilon = light.InnerCone - light.OuterCone;
    float spotIntensity = saturate((theta - light.OuterCone) / max(epsilon, 1e-4));

    float attenuation = Attenuation(light.Attenuation, distance, light.Range);

    return DiffuseBRDF(N, V, L, albedo, metallic, roughness) * attenuation * spotIntensity * light.Intensity * light.Color;
}

inline float CalculatePostProcessMask(Texture2D<uint> customDepthTexture, float2 uv)
{
    PostProcessData data = bit32_6_postProcessData;
    
    uint mask = customDepthTexture.Load(int3(uv * data.ScreenSize, 0));
    float result = min(data.PostProcessMask & mask, 1);
    
    return result;
}

inline float3 RimLight(float3 N, float3 V, float rimPower, float rimIntensity)
{
    float rim = saturate(1.0f - dot(N, V));
    rim = pow(rim, rimPower);
    return rim * rimIntensity;
}

inline float3 CalculateIBL(float3 N, float3 V, TextureCube irradianceMap, TextureCube prefilterMap, Texture2D brdfMap, float3 albedo, float roughness, float metalness)
{    
    float3 F0 = lerp(Fdielectric, albedo, metalness);
    float3 irradiance = irradianceMap.SampleLevel(samLinear_wrap, N,0).rgb;
    
    float NdotV = max(0, dot(N, V));
    
    uint width, height, levels;
    prefilterMap.GetDimensions(0, width, height, levels);

    float3 Lr = 2.0 * NdotV * N - V;
    float3 preFilteredColor = prefilterMap.SampleLevel(samLinear_wrap, Lr, roughness * levels).rgb;
    float2 brdf = brdfMap.SampleLevel(samLinear_clamp, float2(NdotV, roughness),0).rg;

    float3 F = FresnelSchlick(NdotV, F0);
    float3 kD = lerp(1.0 - F, 0, metalness);
    float3 diffuseIBL = kD * albedo * irradiance;
    float3 specularIBL = (F0 * brdf.x + brdf.y) * preFilteredColor;   

    
    return diffuseIBL + specularIBL;
}
float ComputeDynamicMipLevel(float distance, float maxMipLevel)
{
    float mipFromDistance = log2(distance + 1e-3);
    return clamp(mipFromDistance, 0.0, maxMipLevel);
}

// miplevel clam함수
uint SafeMipLevel(float requestedMip, Texture2D tex)
{
    // 0 레벨에서 해상도(width, height)와 전체 mipLevels 수를 얻는다
    uint width, height, mipLevels;
    tex.GetDimensions(0, width, height, mipLevels);

    // floor 후 uint 변환하고, (mipLevels - 1) 과 비교해 clamp
    uint mip = min((uint) floor(requestedMip), mipLevels - 1);
    return mip;
}

inline float CalculateShadow(float3 worldPosition, float3 normal, float3 lightDirection, Texture2DArray shadowMap)
{
    float eyeZ = mul(float4(worldPosition, 1), cameraData.View).z;

    // Static mesh cascade shadow 계산
    uint cid = (eyeZ < cascadeData.CascadeSplits[0]) ? 0 :
               (eyeZ < cascadeData.CascadeSplits[1]) ? 1 : 2;

    float4 staticShadowPos = mul(float4(worldPosition, 1), cascadeData.ShadowVP[cid]);
    staticShadowPos.xyz /= staticShadowPos.w;
    staticShadowPos.xy = staticShadowPos.xy * 0.5f + 0.5f;
    staticShadowPos.y = 1 - staticShadowPos.y;

    // Skeletal mesh single shadow 계산
    float4 skeletalShadowPos = mul(float4(worldPosition, 1), cascadeData.ShadowVP[MAX_CASCADES]);
    skeletalShadowPos.xyz /= skeletalShadowPos.w;
    skeletalShadowPos.xy = skeletalShadowPos.xy * 0.5f + 0.5f;
    skeletalShadowPos.y = 1 - skeletalShadowPos.y;

    static const float2 texelSize = 1.0f / 2048.0f;
    
    // Skeletal shadow는 더 정밀한 bias 사용 (발밑 그림자를 위해)
    float staticBias = max(0.005f * (1.0f - dot(normal, lightDirection)), 0.0005f);
    float skeletalBias = max(0.001f * (1.0f - dot(normal, lightDirection)), 0.0001f); // 훨씬 작은 bias
    
    float staticShadow = 0.0f;
    float skeletalShadow = 0.0f;

    [unroll]
    for (int x = -1; x <= 1; ++x)
    {
        [unroll]
        for (int y = -1; y <= 1; ++y)
        {
            float2 offset = float2(x, y) * texelSize;
            staticShadow += shadowMap.SampleCmpLevelZero(samComparisonLinear_border,  float3(staticShadowPos.xy + offset, cid),  staticShadowPos.z - staticBias);
            skeletalShadow += shadowMap.SampleCmpLevelZero(samComparisonLinear_border, float3(skeletalShadowPos.xy + offset, MAX_CASCADES),  skeletalShadowPos.z - skeletalBias);
        }
    }
    
    staticShadow /= 9.0f;
    skeletalShadow /= 9.0f;

    return min(staticShadow, skeletalShadow);
}

float4 SampleCalculateMipLevel(Texture2D tex, SamplerState sam, float2 uv, float mipLevel)
{
    uint safeMip = SafeMipLevel(mipLevel, tex);
    
    return tex.SampleLevel(sam, uv, safeMip);
}

float3 ReconstructWorldPos(float2 uv, float depth, matrix InvViewProj)
{
    uv.y = 1.0f - uv.y; // Flip Y
    float4 clipPos = float4(uv * 2 - 1, depth, 1);
    float4 worldPos = mul(clipPos, InvViewProj);
    return worldPos.xyz / worldPos.w;
}

float3 ReconstructViewPos(float2 uv, float depth)
{
    uv.y = 1.0f - uv.y; // Flip Y coordinate for NDC
    float2 ndc = uv * 2.0f - 1.0f; // NDC [-1, 1]
    float4 clipPos = float4(ndc, depth, 1.0f);
    float4 viewPos = mul(clipPos, cameraData.ProjectionInverse);
    return viewPos.xyz / viewPos.w;
}

float2 ProjectToUV(float3 viewPos)
{
    float4 clipPos = mul(float4(viewPos, 1.0f), cameraData.Projection);
    float2 ndc = clipPos.xy / clipPos.w;
    float2 uv = ndc * 0.5f + 0.5f; // Convert NDC [-1, 1] to UV [0, 1]
    uv.y = 1.0f - uv.y; // Flip Y coordinate for UV
    return uv;
}

// volumetric fog function
float LinearToExponentialDepth(float z, float nearPlaneZ, float farplaneZ)
{
    float z_buffer_params_y = farplaneZ / nearPlaneZ;
    float z_buffer_params_x = 1.f - z_buffer_params_y;
    
    return (1.f / z - z_buffer_params_y) / z_buffer_params_x;
}

float ExponentialToLinearDepth(float z, float n, float f)
{
    float z_buffer_params_y = f / n;
    float z_buffer_params_x = 1.0f - z_buffer_params_y;

    return 1.0f / (z_buffer_params_x * z + z_buffer_params_y);
}

float3 GetWorldPosFromVoxelID(uint3 texCoord, float jitter, float near, float far, float4x4 invViewProj, float3 volumeSize)
{
    float viewZ = near * pow(far / near, min((float(texCoord.z) + 0.5f + jitter) / volumeSize.z, 1.0f));
    float3 uv = float3((float(texCoord.x) + 0.5f) / volumeSize.x, (float(texCoord.y) + 0.5f) / volumeSize.y, viewZ / far);
    
    float3 ndc;
    ndc.x = 2.0f * uv.x - 1.0f;
    ndc.y = 1.0f - 2.0f * uv.y; //turn upside down for DX
    ndc.z = 2.0f * LinearToExponentialDepth(uv.z, near, far) - 1.0f;
    
    float4 worldPos = mul(float4(ndc, 1.0f), invViewProj);
    worldPos = worldPos / worldPos.w;
    return worldPos.rgb;
}

float3 GetUVFromVolumetricFogVoxelWorldPos(float3 worldPos, float n, float f, matrix viewProj, float3 volumeSize)
{
    float4 ndc = mul(float4(worldPos, 1.0f), viewProj);
    if (ndc.w > 0.0f)
        ndc /= ndc.w;
    
    float3 uv;
    uv.x = ndc.x * 0.5f + 0.5f;
    uv.y = 0.5f - ndc.y * 0.5f; //turn upside down for DX
    uv.z = ExponentialToLinearDepth(ndc.z * 0.5f + 0.5f, n, f);
    
    float2 params = float2(volumeSize.z / log2(f / n), -(volumeSize.z * log2(n) / log2(f / n)));
    float view_z = uv.z * f;
    uv.z = (max(log2(view_z) * params.x + params.y, 0.0f)) / volumeSize.z;
    return uv;
}

// 위상함수
float HenyeyGreensteinPhaseFunction(float3 viewDir, float3 lightDir, float g)
{
    float cosTheta = dot(viewDir, lightDir);
    float denom = 1.f + pow(g, 2) + (2.f * g * cosTheta);
    return (1.f / (4.f * PI)) * (1.f - pow(g, 2)) / max(pow(denom, 1.5f), Epsilon);
}

// TBN 직교정규화: tangent를 normal에 대해 그램-슈미트, bitangent는 cross로 재구축
void OrthonormalizeTBN(inout float3 T, inout float3 B, inout float3 N)
{
    N = normalize(N);
    T = normalize(T - N * dot(T, N));
    B = normalize(cross(N, T));
}

uint OITAllocNode(RWByteAddressBuffer nodeCounter)
{
    uint old;
    nodeCounter.InterlockedAdd(0, 1, old);
    return old;
}

float4 Premultiply(float4 color)
{
    color.rgb *= color.a;
    return color;
}

float CalculatePointLightShadowPCF(float3 fragPos, float3 lightPos, uint lightIndex,
                                   Texture2D pointLightShadowMaps,
                                   float farPlane, float atlasSize, float faceSize)
{
    float3 fragToLight = fragPos - lightPos;
    float currentDistance = length(fragToLight);
    if(currentDistance >= farPlane)
        return 1.0f;
    
    float3 dir = normalize(fragToLight);
    
    float3 absDir = abs(dir);
    float maxAxis = max(absDir.x, max(absDir.y, absDir.z));
    
    uint faceIndex = 0;
    float2 uv = float2(0.0f, 0.0f);
    
    if(maxAxis == absDir.x)
    {
        if(dir.x > 0)
        {
            faceIndex = 0; // +X
            uv = float2(-dir.z, -dir.y) / absDir.x;
        }
        else
        {
            faceIndex = 1; // -X
            uv = float2(dir.z, -dir.y) / absDir.x;
        }
    }
    else if(maxAxis == absDir.y)
    {
        if(dir.y > 0)
        {
            faceIndex = 2; // +Y
            uv = float2(dir.x, dir.z) / absDir.y;
        }
        else
        {
            faceIndex = 3; // -Y
            uv = float2(dir.x, -dir.z) / absDir.y;
        }
    }
    else
    {
        if(dir.z > 0)
        {
            faceIndex = 4; // +Z
            uv = float2(dir.x, -dir.y) / absDir.z;
        }
        else
        {
            faceIndex = 5; // -Z
            uv = float2(-dir.x, -dir.y) / absDir.z;
        }
    };
    
    uv = uv * 0.5f + 0.5f;
    
    uint tilesPerRow = (uint)atlasSize / faceSize;
    uint atlasIndexInGrid = lightIndex * 6 + faceIndex;
    uint atlasX = atlasIndexInGrid % tilesPerRow;
    uint atlasY = atlasIndexInGrid / tilesPerRow;
    float shadow = 0.0;
    float bias = 0.05 + (currentDistance / farPlane) * 0.02;
    float texelSize = 1.0 / faceSize;
    
    
    [unroll]
    for (int x = -1; x <= 1; ++x)
    {
             [unroll]
        for (int y = -1; y <= 1; ++y)
        {
            float2 sampleUV = uv + float2(x, y) * texelSize;
            float2 atlasUV = float2(atlasX * faceSize + sampleUV.x * faceSize,
                                         atlasY * faceSize + sampleUV.y * faceSize) / atlasSize;

            float sampledDepth = pointLightShadowMaps.SampleLevel(samLinear_clamp, atlasUV, 0).r;
            float closestDistance = sampledDepth * farPlane;

            shadow += (currentDistance - bias) > closestDistance ? 0.0 : 1.0;
        }
    }

    return shadow / 9.0;
}

#endif