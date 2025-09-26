#include "CommonData.hlsli"
#include "Function.hlsli"

// Constant buffer
ConstantBuffer<SSGIData> ssgiData;

// Input textures
Texture2D<float> screenDepth;
Texture2D<float4> screenNormal;
Texture2D<float4> screenAlbedo;
Texture2D<float4> screenColor;

// Output UAV
RWTexture2D<float4> SSGI_Result;

[numthreads(16, 16, 1)]
void cs_main(uint3 dtid : SV_DispatchThreadID)
{
    int2 pixelCoordHalf = int2(dtid.xy);

    float fullW = ssgiData.ScreenSize.x;
    float fullH = ssgiData.ScreenSize.y;
    
    float2 uvFull = (float2(dtid.xy) * 2.0f + 1.0f) / ssgiData.ScreenSize;

    // Depth 샘플링
    float depth = screenDepth.SampleLevel(samLinear_clamp, uvFull, 0).r;
    if (depth >= 1.0f)
    {
        SSGI_Result[dtid.xy] = float4(0, 0, 0, 0);
        return;
    }

    // View-space 위치와 normal
    float3 viewPos = ReconstructViewPos(uvFull, depth);
    float3 normal = normalize(screenNormal.SampleLevel(samLinear_clamp, uvFull, 0).xyz);
    float3 albedo = screenAlbedo.SampleLevel(samLinear_clamp, uvFull, 0).xyz;

    // Tangent space 생성
    float3 up = abs(normal.z) < 0.999f ? float3(0, 0, 1) : float3(1, 0, 0);
    float3 tangent = normalize(cross(up, normal));
    float3 bitangent = cross(normal, tangent);

    float3 indirect = float3(0, 0, 0);
    float totalW = 0.0f;

    int numSamples = min(ssgiData.NumSample, 16);
    float radius = ssgiData.Radius;

    [unroll]
    for (int i = 0; i < 16; ++i)
    {
        if (i >= numSamples)
            break;

        float3 k = RandomKernel[i];
        float3 sampleDir = normalize(k.x * tangent + k.y * bitangent + k.z * normal);

        float3 marchPos = viewPos;
        float step = radius * 0.12f;
        float maxDist = radius;

        for (int ms = 0; ms < 32; ++ms)
        {
            marchPos += sampleDir * step;

            float2 sampleUV = ProjectToUV(marchPos);

            float sampleDepth = screenDepth.SampleLevel(samLinear_clamp, sampleUV, 0).r;
            float3 sampleViewPos = ReconstructViewPos(sampleUV, sampleDepth);
            float dist = length(sampleViewPos - marchPos);

            if (dist <= ssgiData.Thickness)
            {
                float3 hitColor = screenColor.SampleLevel(samLinear_clamp, sampleUV, 0).xyz;
                float3 sampleNormal = normalize(screenNormal.SampleLevel(samLinear_clamp, sampleUV, 0).xyz);

                float w1 = max(0.0f, dot(normal, sampleDir));
                float w2 = max(0.0f, dot(sampleNormal, -sampleDir));
                float weight = w1 * w2;

                indirect += hitColor * weight;
                totalW += weight;
                break;
            }

            step += radius * 0.06f;
            if (length(marchPos - viewPos) > maxDist)
                break;
        }
    }

    float3 outputColor = (totalW > 0.0f) ? (indirect / totalW) * ssgiData.Intencity : float3(0, 0, 0);
    outputColor *= albedo;

    SSGI_Result[dtid.xy] = float4(outputColor, 1.0f);
}
