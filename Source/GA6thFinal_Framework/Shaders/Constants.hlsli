#ifndef __Constants__
#define __Constants__

static const float PI = 3.141592654;
static const float3 Fdielectric = 0.04;
static const float2 offsets[9] =
{
    float2(-1.0, -1), float2(0, -1), float2(1, -1),
    float2(-1.0,  0), float2(0,  0), float2(1,  0),
    float2(-1.0,  1), float2(0,  1), float2(1,  1)
};

#endif