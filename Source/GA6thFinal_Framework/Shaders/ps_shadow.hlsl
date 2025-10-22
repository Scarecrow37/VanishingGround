#include "CommonData.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    
    nointerpolation uint4 materialID : TEXCOORD1;
};

struct Material
{
    uint ID[4];
};

struct MipBias
{
    float MipBias;
};

ConstantBuffer<MipBias> bit32_1_mipBias;
Texture2D textures[];

#define DIFFUSE 0

void ps_main(PSInput input)
{    
    float mipBias = bit32_1_mipBias.MipBias;
    uint diffuseID = input.materialID[DIFFUSE];
    
    float alpha = textures[diffuseID].SampleBias(samLinear_wrap, input.uv, mipBias).a;

    clip(alpha - CUTOFF);
}