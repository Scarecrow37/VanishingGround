#include "CommonData.hlsli"
struct RibbonVertexCount
{
    uint count;
};
//ConstantBuffer<RibbonVertexCount> bit32_1_ribbonVertexCount;

StructuredBuffer<uint> ribbonIndices;


struct ParticleOutput
{
    float4 position; //ribbon -> normal
    float4x4 FinalMatrix;
    float4 Color;
    float4 FrameInfo; // ribbon-> x = ribbon width
    int EmitterIndex;
    float3 paddings;
    
};
StructuredBuffer<ParticleOutput> particleInfo;

struct VSOutput
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

VSOutput vs_main(uint vertexID : SV_VertexID)
{
    VSOutput o = (VSOutput) 0;
    //int totalcount = bit32_1_ribbonVertexCount.count / 2;

    uint current_idx = vertexID / 2;
    uint particleIndex = ribbonIndices[current_idx];
    ParticleOutput p = particleInfo[particleIndex];
    float3 pos_curr = p.position.xyz;


    float3 offsetvector = normalize(p.paddings);
    

    int isTop = (vertexID % 2 == 0) ? 1 : -1;
    float ribbonHalfWidth = p.FrameInfo.x * 0.5f;

    // --- 가장 기본적인 월드 위치 계산 ---
    float3 final_world_pos = pos_curr + (offsetvector * ribbonHalfWidth * isTop);

    // --- 표준 파이프라인으로 변환 ---
    float4 viewPos = mul(float4(final_world_pos, 1.0f), cameraData.View);
    o.position = mul(viewPos, cameraData.Projection);

    // UV 및 나머지 데이터
    float u = p.FrameInfo.z;
    float v = (float)(vertexID % 2);
    o.uv = float2(u, v);

    o.color = p.Color;
    o.depth = o.position.z / o.position.w;
    o.emitterIndex = p.EmitterIndex;

    return o;
}