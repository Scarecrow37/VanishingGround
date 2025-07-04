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

enum class MeshRenderType { STATIC, SKELETAL, };

enum GBuffer { BASECOLOR, NORMAL, ORM, EMISSIVE, WORLDPOSITION, DEPTH, CUSTOMDEPTH, GBUFFER_END };

enum CommandListType { COMPUTE_LIST, RENDER_LIST, DEBUG_RENDER_LIST, IMGUI_RENDER_LIST, COMMAND_LIST_END };

enum FenceType { COMPUTE_FENCE, RENDER_FENCE, FENCE_END };

enum PostProcess
{
    BLOOM = 1 << 0,
    OUTLINE = 1 << 1,
};