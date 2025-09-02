#pragma once

constexpr UINT SWAPCHAIN_BUFFER_COUNT = 4;
// volumetric fog medium
constexpr UINT VOXEL_VOLUME_SIZEX      = 160;
constexpr UINT VOXEL_VOLUME_SIZEY      = 90;
constexpr UINT VOXEL_VOLUME_SIZEZ      = 128;
//// volumetric fog high
//constexpr UINT VOXEL_VOLUE_SIZEX = 240;
//constexpr UINT VOXEL_VOLUE_SIZEY = 135;


enum class FeatureLevel { LEVEL_11_0, LEVEL_12_0, LEVEL_12_1 };

enum class ResourceType { MODEL, TEXTURE, ANIMATION };

enum class CommandType { DIRECT, BUNDLE, COMPUTE};

enum CommandQueueType { GRAPHICS_QUEUE, COMPUTE_QUEUE, COPY_QUEUE, COMMAND_QUEUE_END };

enum MeshType { STATIC_MESH, SKELETAL_MESH, MESH_TYPE_END };

enum SpriteType { MODE_2D, MODE_25D, MODE_3D, SPRITE_TYPE_END };

enum PostProcess
{
    BLOOM = 1 << 0,
    OUTLINE = 1 << 1,
};

enum RenderTechniqueFlag : unsigned long long
{
    NONE                = 0,
    SKY_BOX_TECH        = 1 << 0,
    PBR_TECH            = 1 << 1,
    BLOOM_TECH          = 1 << 2,
    UI_TECH             = 1 << 3,
    FONT_TECH           = 1 << 4,
    EDITOR_DRAW_TECH    = 1 << 5,
    PARTICLE_TECH       = 1 << 6,
    RAY_TRACING_TECH    = 1 << 7,
    SSR_TECH            = 1 << 8,
    VOLUMETRIC_FOG_TECH = 1 << 9,
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