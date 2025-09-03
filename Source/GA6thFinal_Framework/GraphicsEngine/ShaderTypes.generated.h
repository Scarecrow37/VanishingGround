#pragma once

// This file is auto-generated. DO NOT EDIT.

namespace GE
{
    enum class VS { 
        NONE,
        GRID,
        PARTICLE_FULLSCREEN,
        PARTICLE_QUAD,
        PARTICLE_RIBBON,
        QUAD,
        SKELETAL_FR,
        SKELETAL_SHADOW_FR,
        SKYBOX,
        STATIC_FR,
        STATIC_SHADOW_FR,
        UI_FR,
        END
     };
    enum class PS { 
        NONE,
        ACCUMULATION,
        BLEND,
        BRIGHT_EXTRACT,
        DISTORTION_RESOLVE,
        DOWN_SAMPLE,
        DXRGBUFFER,
        GAUSSIANBLUR_X,
        GAUSSIANBLUR_Y,
        GBUFFER,
        GRID,
        OUTLINE,
        PARTICLE_QUAD,
        PARTICLE_RESOLVE,
        PBR_LIGHTING,
        SKYBOX,
        SSR,
        TO_BACKBUFFER,
        UI,
        UP_SAMPLE,
        WRITE_AO,
        END
     };
    enum class CS { 
        NONE,
        BRDF_LUT,
        COMPUTE_RIBBON,
        COMPUTE_SPRITE,
        CUBE_TEXTURE_CONVERTOR,
        IRRADIANCE_MAP,
        PREFILTERED_MAP,
        SKELETAL_SKINNING,
        END
     };
    enum class GS { NONE, END };
    enum class HS { NONE, END };
    enum class DS { NONE, END };
}