#pragma once
#define MAX_PARTICLE 1000000
#define PROP_DECL_INIT(type, varName, initValue)                                                                       \
private:                                                                                                               \
    type varName = initValue;

#define PROP_GET(type, varName, FuncName)                                                                              \
public:                                                                                                                \
    type Get##FuncName() const                                                                                         \
    {                                                                                                                  \
        return varName;                                                                                                \
    }

#define PROP_GET_REF(type, varName, FuncName)                                                                          \
public:                                                                                                                \
    const type& Get##FuncName() const                                                                                  \
    {                                                                                                                  \
        return varName;                                                                                                \
    }

#define PROP_SET(type, varName, FuncName)                                                                              \
public:                                                                                                                \
    void Set##FuncName(type value)                                                                                     \
    {                                                                                                                  \
        varName = value;                                                                                               \
    }

#define PROP_GET_SET(type, varName, FuncName)                                                                          \
public:                                                                                                                \
    type Get##FuncName() const                                                                                         \
    {                                                                                                                  \
        return varName;                                                                                                \
    }                                                                                                                  \
    void Set##FuncName(type value)                                                                                     \
    {                                                                                                                  \
        varName = value;                                                                                               \
    }

#define SAFE_RELEASE(ptr)                                                                                              \
    if (ptr)                                                                                                           \
    {                                                                                                                  \
        ptr->Release();                                                                                                \
        ptr = nullptr;                                                                                                 \
    }

#include <DirectXColors.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <queue>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
using namespace DirectX::SimpleMath;

struct Particle
{
    Vector3 Position;
    Vector3 Velocity;
    Vector4 Color;
    float   LifeTime;
    float   Age;
    float   Size;
    float   Rotation;
    float   Padding;
};

struct EmitterInfo
{
    Matrix  World;
    Matrix  Rotation;
    Vector4 StartColor;
    Vector4 EndColor;
    Vector4 StartEndSize;  // x: start, y: end
    Vector4 StartEndRot;   // x: start, y: end
    Vector4 StartEndLife;  // x: start, y: end
    Vector4 StartEndSpeed; // x: start, y: end
    Vector4 AlbedoIndex;   // x: index, y: count
    Vector4 Flags;         // custom flags
    Vector4 Misc;          // user
};

struct RibbonIndex
{
    UINT Top;
    UINT Bottom;
};

struct MVP
{
    Matrix  ViewProjMatrix;
    Matrix  ViewRotInvMatrix;
    Vector4 CameraPos;
    Vector4 Time; // x: delta, y: total
};

struct ParticleUpdateResource
{
    std::string Name;

    // unique_ptr -> shared_ptr 로 변경 (씬이 공유 소유)
    std::vector<std::shared_ptr<class ParticleEffect>> SceneEffects;

    std::vector<class Particle> TotalParticles;
    std::vector<EmitterInfo>    EmitterMatrix;
    std::vector<UINT>           ActiveEmitterAlbedos;
    UINT                        TotalCount = 0;

    std::vector<class Particle>           RibbonTotalParticles;
    std::vector<EmitterInfo>              RibbonEmitterMatrix;
    std::vector<UINT>                     RibbonActiveEmitterAlbedos;
    std::vector<std::vector<RibbonIndex>> RibbonIndices;
    UINT                                  RibbonTotalCount = 0;

    ComPtr<ID3D12Resource> ParticleInput;
    ComPtr<ID3D12Resource> EmitterInfo;
    ComPtr<ID3D12Resource> ParticleInputUpload;
    ComPtr<ID3D12Resource> EmitterInfoUpload;
    ComPtr<ID3D12Resource> RibbonParticleInput;
    ComPtr<ID3D12Resource> RibbonEmitterInfo;
    ComPtr<ID3D12Resource> RibbonParticleInputUpload;
    ComPtr<ID3D12Resource> RibbonEmitterInfoUpload;
};

struct ParticleRenderResource
{
    std::string            Name;
    ComPtr<ID3D12Resource> SimulationOutput;
    ComPtr<ID3D12Resource> RibbonSimulationOutput;
    ComPtr<ID3D12Resource> MvpConstant;
};

struct ParticleSceneResource
{
    std::string                             Name;
    ComPtr<ID3D12GraphicsCommandList>       CommandList;
    ComPtr<ID3D12CommandAllocator>          CommandAllocator;
    std::shared_ptr<ParticleUpdateResource> UpdateParticleResource;
    std::unique_ptr<ParticleRenderResource> RenderParticleResource;
};
