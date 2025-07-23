#pragma once

constexpr UINT MAX_BONE_MATRIX        = 128;
constexpr UINT SWAPCHAIN_BUFFER_COUNT = 4;
constexpr UINT MAX_DIRECTIONAL_LIGHT  = 4;
constexpr UINT MAX_POINT_LIGHT        = 32;
constexpr UINT MAX_SPOT_LIGHT         = 16;
constexpr UINT MAX_LIGHT              = MAX_DIRECTIONAL_LIGHT + MAX_POINT_LIGHT + MAX_SPOT_LIGHT;
constexpr UINT MAX_MIPMAP_LEVEL       = 2;

enum class FeatureLevel { LEVEL_11_0, LEVEL_12_0, LEVEL_12_1 };

enum class ResourceType { MODEL, TEXTURE, ANIMATION };

enum class CommandType { DIRECT, BUNDLE, COMPUTE};

enum MeshType { STATIC_MESH, SKELETAL_MESH, MESH_TYPE_END };

enum GBuffer { BASECOLOR, NORMAL, ORM, EMISSIVE, WORLDPOSITION, DEPTH, CUSTOMDEPTH, GBUFFER_END };

enum DXRGBuffer { DXRNORMAL, DXRWORLDPOSITION, DXRDEPTH, DXRCUSTOMDEPTH, DXRGBUFFER_END };

enum CommandQueueType { GRAPHICS_QUEUE, COMPUTE_QUEUE, COPY_QUEUE, COMMAND_QUEUE_END };

enum FrameResourceType { TRANSFORM, BONE_MATRICES, MATERIAL, UI_TRANSFORM, UI_MATERIAL, VERTEX_BUFFER_ID, INDEX_BUFFER_ID, STATIC_MESH_INSTANCE_ID, SKELETAL_MESH_INSTANCE_ID, FRAME_TYPE_END };

enum SpriteType { MODE_2D, MODE_25D, MODE_3D, SPRITE_TYPE_END };

enum PostProcess
{
    BLOOM = 1 << 0,
    OUTLINE = 1 << 1,
};

enum RenderTechniqueFlag : unsigned long long
{
    NONE             = 0,
    SKY_BOX_TECH     = 1 << 0,
    PBR_TECH         = 1 << 1,
    BLOOM_TECH       = 1 << 2,
    UI_TECH          = 1 << 3,
    FONT_TECH        = 1 << 4,
    EDITOR_DRAW_TECH = 1 << 5,
    PARTICLE_TECH    = 1 << 6,
    RAY_TRACING_TECH = 1 << 7,
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