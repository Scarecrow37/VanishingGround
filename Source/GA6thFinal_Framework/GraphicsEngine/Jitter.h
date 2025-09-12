#pragma once
#include <cstdint>

inline float Halton(uint32_t index, uint32_t base)
{
    float f = 1.0f, r = 0.0f;
    while (index > 0)
    {
        f /= base;
        r += f * (index % base);
        index /= base;
    }
    return r;
}

struct CameraJitter
{
    uint32_t frameIndex = 0;
    float    jitterX_px = 0.0f; // pixels @ render resolution
    float    jitterY_px = 0.0f;

    void Next(uint32_t renderW, uint32_t renderH, float scale = 1.0f)
    {
        frameIndex++;
        float jx   = (Halton(frameIndex, 2) - 0.5f) * scale;
        float jy   = (Halton(frameIndex, 3) - 0.5f) * scale;
        jitterX_px = jx;
        jitterY_px = jy;
    }

    // 투영행렬에 적용할 NDC 오프셋(행렬 _31/_32에 더함)
    static void ApplyToProjection(Matrix& proj, float jitterX_px, float jitterY_px, uint32_t renderW, uint32_t renderH)
    {
        const float ox = (jitterX_px * 2.0f) / float(renderW);
        const float oy = (jitterY_px * -2.0f) / float(renderH);
        proj._31 += ox;
        proj._32 += oy;
    }
};