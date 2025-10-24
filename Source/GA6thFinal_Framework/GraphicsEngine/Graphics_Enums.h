#pragma once

constexpr UINT SWAPCHAIN_BUFFER_COUNT = 4;

enum class FeatureLevel { LEVEL_11_0, LEVEL_12_0, LEVEL_12_1 };

enum class ResourceType { MODEL, TEXTURE, ANIMATION };

enum class CommandType { DIRECT, BUNDLE, COMPUTE};

enum CommandQueueType { GRAPHICS_QUEUE, COMPUTE_QUEUE, COPY_QUEUE, COMMAND_QUEUE_END };

enum MeshType { STATIC_MESH, SKELETAL_MESH, MESH_TYPE_END };

enum SpriteType { MODE_3D, MODE_25D, MODE_2D, SPRITE_TYPE_END };

enum UIMaterialType { DEFAULT, LINEAR_FILL };

enum PostProcess
{
    BLOOM            = 1 << 0,
    OUTLINE          = 1 << 1,
    IS_SKELETAL_MESH = 1 << 2,
};

enum class CustomLightType
{
    NONE,
    TRANSPARENT_RIM_LIGHT,
    END
};

enum RenderTechniqueFlag : unsigned long long
{
    NONE                  = 0,
    SKY_BOX_TECH          = 1 << 0,
    LIGHTING_TECH         = 1 << 1,
    BLOOM_TECH            = 1 << 2,
    UI_TECH               = 1 << 3,
    FONT_TECH             = 1 << 4,
    EDITOR_DRAW_TECH      = 1 << 5,
    PARTICLE_TECH         = 1 << 6,
    RAY_TRACING_TECH      = 1 << 7,
    SSR_TECH              = 1 << 8,
    DISTORTION_TECH       = 1 << 9,
    VOLUMETRIC_FOG_TECH   = 1 << 10,
    SCENE_TRANSITION_TECH = 1 << 11,
    SSGI_TECH             = 1 << 12,
    FXAA_TECH             = 1 << 13,
};

inline RenderTechniqueFlag operator|(RenderTechniqueFlag lhs, RenderTechniqueFlag rhs)
{
    return static_cast<RenderTechniqueFlag>(static_cast<unsigned long long>(lhs) | static_cast<unsigned long long>(rhs));
}

inline RenderTechniqueFlag& operator|=(RenderTechniqueFlag& lhs, RenderTechniqueFlag rhs)
{
    lhs = lhs | rhs;
    return lhs;
}