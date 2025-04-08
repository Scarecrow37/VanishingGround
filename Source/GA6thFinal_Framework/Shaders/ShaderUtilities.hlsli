#ifndef __ShaderUtilities__
#define __ShaderUtilities__

inline float3 GammaToLinearSpace(float3 sRGB)
{
    return sRGB * (sRGB * (sRGB * 0.305306011 + 0.682171111) + 0.012522878);
}

inline float3 LinearToGammaSpace(float3 linRGB)
{
    linRGB = max(linRGB, float3(0.0, 0.0, 0.0));
    return max(1.055 * pow(linRGB, 0.416666667) - 0.055, 0.0);
}

#endif