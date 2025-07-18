#include "Compute.hlsli"

struct RibbonVertexCount
{
    uint count;
};
ConstantBuffer<RibbonVertexCount> bit32_1_ribbonVertexCount;

StructuredBuffer<uint> ribbonIndices;
StructuredBuffer<ParticleOutput> particleInfo;

struct VSInput
{
    uint vertexID : SV_VertexID; // 인스턴스 ID 추가

};

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

VSOutput vs_main(VSInput input)
{

    VSOutput o = (VSOutput) 0;
    int totalcount = bit32_1_ribbonVertexCount.count / 2;
    
    uint particleIndex = ribbonIndices[input.vertexID/2];
    uint particleIndexnext = ribbonIndices[min(input.vertexID / 2 + 2,totalcount)];
    
    int isTop = input.vertexID % 2; // Even: top, Odd: bottom
    isTop *= -2;
    isTop += 1;
    
    ParticleOutput p = particleInfo[particleIndex];
    ParticleOutput np = particleInfo[particleIndexnext];

    float4 currentvertex = float4(0, isTop , 0, 1);
    float3 offsetDir = normalize(p.position.xyz);
    float3 centerWorldPos = float3(p.paddings);
    float3 centerWorldPosnext = float3(np.paddings);
    
    float3 progressDir = normalize(centerWorldPosnext - centerWorldPos);
    float3 offsetvector = cross(offsetDir, max(progressDir, float3(1, 0, 0)));
    float ribbonHalfWidth = p.FrameInfo.x;
    
    float4 newpos = float4(offsetvector * ribbonHalfWidth *isTop, 1);
    o.position = mul(newpos, p.FinalMatrix);
    uint ratio = input.vertexID / 2;
    float u = float(ratio) / float(totalcount - 1);
    o.uv = float2(p.FrameInfo.z, (1 + isTop) * 0.5f); // 필요 시 u좌표는 셰이더에서 계산
    
    o.color = p.Color;
    
    o.depth = o.position.z / o.position.w;
    
    return o;
    
}