#include "PostProcess.hlsli"
#include "ShaderUtilities.hlsli"

#ifdef Sampling
Texture2D txDepth   : register(t2);
Texture2D txNormal  : register(t3);
Texture2D txNoise   : register(t4);

#define SAMPLES 16
static const float3 sample_sphere[16] =
{
    float3(0.5381, 0.1856, -0.4319), float3(0.1379, 0.2486, 0.4430),
    float3(0.3371, 0.5679, -0.0057), float3(-0.6999, -0.0451, -0.0019),
    float3(0.0689, -0.1598, -0.8547), float3(0.0560, 0.0069, -0.1843),
    float3(-0.0146, 0.1402, 0.0762), float3(0.0100, -0.1924, -0.0344),
    float3(-0.3577, -0.5301, -0.4358), float3(-0.3169, 0.1063, 0.0158),
    float3(0.0103, -0.5869, 0.0046), float3(-0.0897, -0.4940, 0.3287),
    float3(0.7119, -0.0154, -0.0918), float3(-0.0533, 0.0596, -0.5411),
    float3(0.0352, -0.0631, 0.5460), float3(-0.4776, 0.2847, -0.0271)
};

//static const float3 sample_sphere[32] =
//{
//    float3(0.5381, 0.1856, -0.4319), float3(0.1379, 0.2486, 0.4430),
//    float3(0.3371, 0.5679, -0.0057), float3(-0.6999, -0.0451, -0.0019),
//    float3(0.0689, -0.1598, -0.8547), float3(0.0560, 0.0069, -0.1843),
//    float3(-0.0146, 0.1402, 0.0762), float3(0.0100, -0.1924, -0.0344),
//    float3(-0.3577, -0.5301, -0.4358), float3(-0.3169, 0.1063, 0.0158),
//    float3(0.0103, -0.5869, 0.0046), float3(-0.0897, -0.4940, 0.3287),
//    float3(0.7119, -0.0154, -0.0918), float3(-0.0533, 0.0596, -0.5411),
//    float3(0.0352, -0.0631, 0.5460), float3(-0.4776, 0.2847, -0.0271),
//    float3(0.4212, -0.4123, 0.5215), float3(-0.2435, 0.7512, -0.1763),
//    float3(0.5692, 0.1258, 0.6571), float3(-0.6611, 0.4243, 0.0245),
//    float3(0.1934, -0.6547, -0.7241), float3(-0.3543, -0.2732, 0.8921),
//    float3(0.4351, -0.3176, -0.7214), float3(-0.2253, 0.3479, 0.7894),
//    float3(0.6824, -0.4792, -0.1823), float3(-0.5967, 0.1032, -0.7895),
//    float3(0.2468, 0.5894, 0.4723), float3(-0.4537, 0.3485, -0.4861),
//    float3(0.1278, -0.4821, 0.6023), float3(-0.6724, -0.2983, -0.5147),
//    float3(0.5834, 0.4327, -0.2895), float3(-0.5231, -0.6123, 0.1872)
//};

float3 GetRandom(float2 uv)
{
    float2 offsetUV =  screenSize * uv / 4.0f;
    return normalize(txNoise.Sample(samLinear_wrap, offsetUV).xyz * 2.0f - 1.0f);
}

float GetViewDepth(float2 uv)
{
    return txDepth.Sample(samLinear_wrap, uv).r;
}

float3 NormalFromDepth(float depth, float2 uv)
{
    const float2 offset1 = float2(0.0, 0.001);
    const float2 offset2 = float2(0.001, 0.0);
   
    float depth1 = GetViewDepth(uv + offset1);
    float depth2 = GetViewDepth(uv + offset2);
   
    float3 p1 = float3(offset1, depth1 - depth);
    float3 p2 = float3(offset2, depth2 - depth);
   
    float3 normal = cross(p1, p2);
    normal.z = -normal.z;
   
    return normalize(normal);
}

float3 normal_from_depth(float depth, float2 texcoords)
{  
  const float2 offset1 = float2(0.0,0.001);
  const float2 offset2 = float2(0.001,0.0);
  
  float depth1 = txDepth.Sample(samLinear_wrap, texcoords + offset1).r;
  float depth2 = txDepth.Sample(samLinear_wrap, texcoords + offset2).r;
  
  float3 p1 = float3(offset1, depth1 - depth);
  float3 p2 = float3(offset2, depth2 - depth);
  
  float3 normal = cross(p1, p2);
  normal.z = -normal.z;
  
  return normalize(normal);
}

#else
Texture2D txSSAO    : register(t2);
Texture2D txAmbient : register(t3);
#endif

float4 main(PS_INPUT input) : SV_Target
{
#ifdef Sampling
    float total_strength = 1;
    float base = 0.2;
    
    float area = 0.015;
    float falloff = 0.000001;
    float radius = 0.0003;
    
    float3 random = GetRandom(input.uv);
    float depth = GetViewDepth(input.uv);
    //depth = (depth - 0.5) * 2;
    
    float3 position = float3(input.uv, depth);
    //float3 normal = txNormal.Sample(samLinear_wrap, input.uv).xyz;
    float3 normal = normal_from_depth(depth, input.uv);
    float radius_depth = radius / depth;
    float occlusion = 0.0;
    
    [unroll]
    for (int i = 0; i < SAMPLES; i++)
    {
        float3 ray = radius_depth * reflect(sample_sphere[i], random);
        float3 hemi_ray = position + sign(dot(ray, normal)) * ray;
      
        float occ_depth = GetViewDepth(saturate(hemi_ray.xy));
        float difference = depth - occ_depth;
      
        //float weight = smoothstep(falloff, area, difference);
        //occlusion += step(falloff, difference) * (1.0 - weight * weight);
        occlusion += step(falloff, difference) * (1.0 - smoothstep(falloff, area, difference));
    }
    
    float ao = 1.0 - total_strength * occlusion * (1.0 / SAMPLES);
    return float4(saturate(ao + base).xxx, 1);
   
#else
    float SSAO = txSSAO.Sample(samLinear_wrap, input.uv).r;
    float4 ambient = txAmbient.Sample(samLinear_wrap, input.uv);
    float3 ambientColor = ambient.rgb * ambient.a * SSAO;
    float3 color = ambientColor + txSource.Sample(samLinear_wrap, input.uv).rgb;
    
    return float4(color, 1);
#endif
}