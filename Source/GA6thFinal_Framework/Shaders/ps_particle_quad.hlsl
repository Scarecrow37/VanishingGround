#include "CommonData.hlsli"
//struct textureIDs
//{
//    int textureID[100];
//};
//ConstantBuffer<textureIDs> texID;

StructuredBuffer<int> texID;

//// UAVs for WBOIT
RasterizerOrderedTexture2D<float4> gAccumTex;
RasterizerOrderedTexture2D<float> gRevealTex;


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
    int emitIndex = input.emitterIndex;
    int albedoID = texID[emitIndex];
    float factor = textures[albedoID].Sample(samPoint_clamp, input.uv);
    float alpha = input.color.a * factor;
    
    // 깊이 기반 가중치 계산 (예: McGuire & Bavoil)
    //float wDepth = pow(saturate(input.depth), 1.5);
    float wDepth = pow(1.0 - saturate(input.depth), 1.5);
    
    float minAlpha = 0.03f;
    float adjAlpha = max(alpha, minAlpha);
    float weight = adjAlpha * max(0.01, min(3000.0,
                   10.0 / (0.00001 + pow(abs(input.depth) / 5.0, 2) +
                                  pow(abs(input.depth) / 200.0, 6))));
    
    gAccumTex[uint2(input.position.xy)] += float4(input.color.rgb * alpha * weight, alpha *weight);
    gRevealTex[uint2(input.position.xy)] += alpha;

    
    float4 output = float4(input.color.rgb, input.color.a * factor);
    return output;
    
    
    
}
