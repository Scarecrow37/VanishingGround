#include "CommonData.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct Material
{
    uint ID[4];
};

struct MipBias
{
    float MipBias;
};

StructuredBuffer<Material> material;
ConstantBuffer<MipBias> bit32_1_mipBias;
Texture2D textures[];

#define DIFFUSE 0

void ps_main(PSInput input)
{
    ShadowObjectData data = bit32_5_shadowObjectData;
    
    float mipBias = bit32_1_mipBias.MipBias;
    uint diffuseID = material[data.ID].ID[DIFFUSE];
    
    float alpha = textures[diffuseID].SampleBias(samLinear_wrap, input.uv, mipBias).a;

    clip(alpha - CUTOFF);
}