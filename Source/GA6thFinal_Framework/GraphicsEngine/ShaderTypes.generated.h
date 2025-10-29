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
        SDF_FONT_FR,
        SKELETAL_FORWARD_FR,
        SKELETAL_FR,
        SKELETAL_POINT_LIGHT_SHADOW_FR,
        SKELETAL_SHADOW_FR,
        SKYBOX,
        STATIC_FORWARD_FR,
        STATIC_FR,
        STATIC_POINT_LIGHT_SHADOW_FR,
        STATIC_SHADOW_FR,
        UI_FR,
        END
     };
    enum class PS { 
        NONE,
        ACCUMULATION,
        BILATERAL_UP_SAMPLE,
        BLEND,
        BRIGHT_EXTRACT,
        BURN_DISSOLVE,
        DISTORTION_RESOLVE,
        DOWN_SAMPLE,
        DXRGBUFFER,
        DXRGBUFFER_MASKED,
        FADE,
        FORWARD_PBR_LIGHTING,
        FXAA,
        GAUSSIANBLUR_X,
        GAUSSIANBLUR_Y,
        GBUFFER,
        GBUFFER_MASKED,
        GRID,
        OUTLINE,
        PARTICLE_QUAD,
        PARTICLE_RESOLVE,
        PBR_LIGHTING,
        POINT_LIGHT_SHADOW,
        SDF_FONT,
        SDF_FONT_OIT,
        SHADOW,
        SKYBOX,
        SSGI_COMPOSITE,
        SSR,
        TO_BACKBUFFER,
        TRANSPARENT_LIM_LIGHT,
        UI,
        UI_OIT,
        UP_SAMPLE,
        VOLUMETRIC_FOG,
        WRITE_AO,
        END
     };
    enum class CS { 
        NONE,
        BRDF_LUT,
        CALCULATE_MOTION_VECTOR,
        COMPUTE_RIBBON,
        COMPUTE_SPRITE,
        CUBE_TEXTURE_CONVERTOR,
        GENERATE_SSGI,
        IRRADIANCE_MAP,
        LIGHT_ACCUMULATION,
        LIGHT_INJECTION,
        PREFILTERED_MAP,
        RESOLVE_UI,
        SKELETAL_SKINNING,
        SSGI_TEMPORAL,
        END
     };
    enum class GS { 
        NONE,
        POINT_LIGHT_SHADOW_CUBE,
        END
     };
    enum class HS { NONE, END };
    enum class DS { NONE, END };
}