#include "CommonData.hlsli"


Texture2D<float> depthbuffer;
StructuredBuffer<int> texID;

//// UAVs for WBOIT
RasterizerOrderedTexture2D<float4> gAccumTex;
RasterizerOrderedTexture2D<float> gRevealTex;

//RWTexture2D<float4> gAccumTex;
//RWTexture2D<float> gRevealTex;

Texture2D textures[];


struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
    float4 color : COLOR0;
    nointerpolation int emitterIndex : CUSTOM_FLAG;
};


uint ps_main(PSInput input) : SV_Target
{
    PostProcessData data = bit32_6_postProcessData;
    
    float d = input.position.z;
    float2 depthBufferUV = input.position.xy * data.TexelSize;
    
    float destDepth = depthbuffer.Sample(samPoint_wrap, depthBufferUV).r;
    clip(destDepth - d);
    
    clip(input.color.a - 0.001f);

    int emitIndex = input.emitterIndex;
    int albedoID = texID[emitIndex];

    float factor = textures[albedoID].Sample(samLinear_wrap, input.uv).r;
    float alpha = input.color.a * factor;

    clip(alpha - 0.01f);
    
    
    float d2 = d * d;
    float d4 = d2 * d2;
    float d6 = d4 * d2; // depth^6 계산 최소화

    // 사전 계산된 상수
    const float denom_const = 0.001f;
    const float div5 = 0.04f; // 1 / 25
    const float div200 = 1.5625e-14f; // 1 / 200^6

    float denom = denom_const + d2 * div5 + d6 * div200;
    float weight = alpha / denom;

    clip(weight - 0.001f);

    float contrib = alpha * weight;
    float3 color = input.color.rgb * contrib;
    uint2 pos = uint2(input.position.xy);
    gAccumTex[pos] += float4(color, contrib);
    gRevealTex[pos] += alpha;

    
    return ceil(alpha - 0.1f);
}