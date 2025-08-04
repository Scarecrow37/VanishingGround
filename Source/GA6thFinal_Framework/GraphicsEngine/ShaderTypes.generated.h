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
        DOWN_SAMPLE,
        DXRGBUFFER,
        GAUSSIANBLUR_X_RTV,
        GAUSSIANBLUR_X_UAV,
        GAUSSIANBLUR_Y_RTV,
        GAUSSIANBLUR_Y_UAV,
        GBUFFER,
        GRID,
        OUTLINE,
        PARTICLE_QUAD,
        PARTICLE_RESOLVE,
        PBR_LIGHTING,
        SKYBOX,
        SSAO,
        TO_BACKBUFFER,
        UI,
        UP_SAMPLE,
        END
     };
    enum class CS { 
        NONE,
        BRDF_LUT,
        COMPUTE_RIBBON,
        COMPUTE_SPRITE,
        CUBE_TEXTURE_CONVERTOR,
        DEPTH_EXTRACT,
        IRRADIANCE_MAP,
        PARTICLE_REORDER,
        PREFILTERED_MAP,
        PREFIX_SUM,
        RADIX_HISTOGRAM,
        RADIX_SCATTER,
        SKELETAL_SKINNING,
        VALIDATE_SORT,
        END
     };
    enum class GS { NONE, END };
    enum class HS { NONE, END };
    enum class DS { NONE, END };
}