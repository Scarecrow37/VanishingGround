#ifndef __SDF_DATAS__
#define __SDF_DATAS__

struct FontColor
{
    float4 Color;
};

struct SDFParams
{
    uint InstanceID;
    uint Flags;
    float PxRange;
    float FontWeight;
    
    // Outline parameters
    float4 OutlineColor;
    float OutlineWidth;
};

ConstantBuffer<FontColor> bit32_4_fontColor;
ConstantBuffer<SDFParams> bit32_9_sdfParams;

#define sdfParams bit32_9_sdfParams

#define ENABLE_OUTLINE (1 << 0)

#endif