#include "CommonData.hlsli"

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
    float depth : TEXCOORD1;
    nointerpolation int emitterIndex : CUSTOM_FLAG;
};

float4 ps_main(PSInput input) : SV_Target
{
    // 1. 텍스처 샘플링 최소화
    int emitIndex = input.emitterIndex;
    int albedoID = texID[emitIndex];
    float factor = textures[albedoID].Sample(samPoint_clamp, input.uv);
    
    // 2. 알파 계산 간소화
    float alpha = input.color.a * factor;
    clip(alpha > 0.01f);
    
    // 3. 가중치 계산 최적화
    float depth = input.depth;
    clip(depth > 0.95f);
    float depth2 = depth * depth; // 제곱 캐싱
    float depth6 = depth2 * depth2 * depth2; // 6제곱 효율적 계산
    
    // 사전 계산된 상수 활용
    static const float denom_const = 0.00001f;
    static const float div5 = 1.0f / 25.0f; // (1/5)^2
    static const float div200 = 1.0f / (200.0f * 200.0f * 200.0f * 200.0f * 200.0f * 200.0f);
    
    float denominator = denom_const +
                       (depth2 * div5) +
                       (depth6 * div200);
    
    float weight = alpha * 10.0f / denominator;
    
    // 4. UAV 쓰기 최적화
    float3 color_contrib = input.color.rgb * alpha * weight;
    float alpha_contrib = alpha * weight;
    
    gAccumTex[uint2(input.position.xy)] += float4(color_contrib, alpha_contrib) * 0.5f;
    gRevealTex[uint2(input.position.xy)] += alpha*0.5f;
    
    // 5. 불필요한 출력 제거
    return float4(color_contrib, alpha_contrib);
    //return 1;

}

