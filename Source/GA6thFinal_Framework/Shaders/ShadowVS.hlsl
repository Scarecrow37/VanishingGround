struct VS_INPUT
{
    float4 position         : POSITION;
    float3 normal           : NORMAL;
    float3 tangent          : TANGENT;
    float3 biTangent        : BITANGENT;
    float2 uv               : TEXCOORD;
    
#ifdef Skeletal
    uint4 blendIndices      : BLENDINDICES0;
    float4 blendWeights     : BLENDWEIGHT0;
#endif
};

struct VS_OUTPUT
{
    float4 position         : SV_POSITION;
};

cbuffer ViewProjection : register(b0)
{
    matrix vp;
    matrix shadowVP;
}

cbuffer ModelIndex : register(b1)
{
    uint index;
    uint offset;
}

StructuredBuffer<matrix> WorldMatrices : register(t0);
StructuredBuffer<matrix> BoneMatrices : register(t1);

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    matrix worldTransform;
    
#ifdef Skeletal
    matrix boneTransform = mul(input.blendWeights.x, BoneMatrices[index * offset + input.blendIndices.x]);
    boneTransform += mul(input.blendWeights.y, BoneMatrices[index * offset + input.blendIndices.y]);
    boneTransform += mul(input.blendWeights.z, BoneMatrices[index * offset + input.blendIndices.z]);
    boneTransform += mul(input.blendWeights.w, BoneMatrices[index * offset + input.blendIndices.w]);
    worldTransform = mul(boneTransform, WorldMatrices[index]);
#else    
    worldTransform = WorldMatrices[index];
#endif
    
    output.position = mul(input.position, worldTransform);
    output.position = mul(output.position, shadowVP);
    
    return output;
}