#pragma once
#include "Graphics_Enums.h"

constexpr UINT MAX_BONE_MATRIX        = 256;
constexpr UINT MAX_DIRECTIONAL_LIGHT  = 4;
constexpr UINT MAX_POINT_LIGHT        = 32;
constexpr UINT MAX_SPOT_LIGHT         = 16;
constexpr UINT MAX_LIGHT              = MAX_DIRECTIONAL_LIGHT + MAX_POINT_LIGHT + MAX_SPOT_LIGHT;
constexpr UINT MAX_MIPMAP_LEVEL       = 5;
constexpr UINT MAX_CASCADES           = 4;

// volumetric fog medium
constexpr UINT VOXEL_VOLUME_SIZEX = 240;
constexpr UINT VOXEL_VOLUME_SIZEY = 135;
constexpr UINT VOXEL_VOLUME_SIZEZ = 128;

enum GBuffer { BASECOLOR, NORMAL, ORM, EMISSIVE, DEPTH, CUSTOMDEPTH, GBUFFER_END };

enum DXRGBuffer { DXRNORMAL, DXRDEPTH, DXRCUSTOMDEPTH, DXRGBUFFER_END };

enum FrameResourceType { TRANSFORM, BONE_MATRICES, MATERIAL, UI_TRANSFORM, UI_MATERIAL, VERTEX_BUFFER_ID, INDEX_BUFFER_ID, MESH_INSTANCE_ID, FRAME_TYPE_END };

// Render technique order
// 순서 보장을 위해서(환경설정에서 on off시 순서가 바뀌지 않도록 RenderScene은 Tech를 map으로 가지고 있음.)
enum TechOrder : int
{
    SKYBOX = 0,
    MESH_LIGHTING=1,
    SSR           = 2,
    VOLUMETRIC_FOG = 3,
    PARTICLE       = 4,
    EDITOR_DRAW    = 5,
    BLOOM          = 6,
    BLEND          = 7,
    UI             = 8,
    FONT           = 9,
};