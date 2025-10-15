#pragma once
#define MAX_PARTICLE 1000000
#define PROP_DECL_INIT(type, varName, initValue)                                                                       \
private:                                                                                                             \
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

#define PROP_SET_REF(type, varName, FuncName)                                                                          \
public:                                                                                                                \
    void Set##FuncName(const type& value)                                                                              \
    {                                                                                                                  \
        varName = value;                                                                                               \
    }

#define UMPARTICLE_PROPERTY(type, varName, FuncName, initValue)                                                        \
    PROP_DECL_INIT(type, varName, initValue)                                                                           \
    PROP_GET(type, varName, FuncName)                                                                                  \
    PROP_SET(type, varName, FuncName)

#define UMPARTICLE_PROPERTY_REF(type, varName, FuncName, initValue)                                                    \
    PROP_DECL_INIT(type, varName, initValue)                                                                           \
    PROP_GET_REF(type, varName, FuncName)                                                                              \
    PROP_SET_REF(type, varName, FuncName)

#define BYTEALIGN(value, alignment) (((value) + ((alignment) - 1)) & ~((alignment) - 1))



struct ParticleOutput
{
    Vector4 position; // ribbon -> normal
    Matrix  FinalMatrix;
    Vector4 Color;
    Vector4 FrameInfo;
    int     EmitterIndex;
    Vector3   paddings;
};

struct EmitterInfo
{
    Matrix  WorldMatrix;
    Matrix  OrientedWorldMatrix;
    Vector4 dragPoint;
    Vector4 dragForce;
    Vector4 vortexForce;
    Vector4 startScale;
    Vector4 endScale;
    Vector4 startColor;
    Vector4 endColor;
    Vector4 lifetime; // x: particle lifetime, y: useWorldSpace (1.0f for true, 0.0f for false)
    Vector4 startNormal;
    Vector4 endNormal;
    Vector4 ribbonVector;
};

struct __declspec(align(16)) MVPConstants
{
    Matrix  ViewMatrix;
    Matrix  ViewRotInvMatrix;
    Matrix  ProjMatrix;
    Vector4 CameraPos;
    float   deltaTime; // 4바이트

    // 패딩을 float 배열로 대체 (44바이트)
    float pad1[4]; // 16바이트 (deltaTime 이후 12바이트 남은 공간 채움)
    float pad2[4]; // 16바이트
    float pad3[3]; // 12바이트 (총 16+16+12 = 44바이트)
};




enum class LocationShape
{
    SPHERE,
    CUBE,
    CYLINDER,
    CONE,
    TORUS,
    MESH_SURFACE
};
enum class ParticleType
{
    SPRITE,
    MESH,
    RIBBON,
    MISC,
}; 
enum ParticleMiscFlag
{
    DISTORTION,
    BLUR,
};

enum class VelocityScaleType
{
    LINEAR,
    POINT,
    CONE,
    CUSTOM
};

struct RibbonIndex
{
    UINT  _index = -1;
    float _ratio = 0;
};

struct ParticleUpdateResource
{
    std::string _name;

    std::vector<std::unique_ptr<class ParticleEffect>> _sceneEffects;

    std::vector<class Particle>           _totalParticles;
    std::vector<EmitterInfo>              _emitterMatrix;
    std::vector<UINT>                     _activeEmitterAlbedos;
    UINT                                  _totalCount = 0;
    std::vector<class Particle>           _ribbonTotalParticles;
    std::vector<EmitterInfo>              _ribbonEmitterMatrix;
    std::vector<UINT>                     _ribbonActiveEmitterAlbedos;
    std::vector<std::vector<RibbonIndex>> _ribbonIndices;
    UINT                                  _ribbonTotalCount = 0;

    ComPtr<ID3D12Resource> _particleInput;
    ComPtr<ID3D12Resource> _emitterInfo;
    ComPtr<ID3D12Resource> _particleInputUpload;
    ComPtr<ID3D12Resource> _emitterInfoUpload;
    ComPtr<ID3D12Resource> _ribbonParticleInput;
    ComPtr<ID3D12Resource> _ribbonEmitterInfo;
    ComPtr<ID3D12Resource> _ribbonParticleInputUpload;
    ComPtr<ID3D12Resource> _ribbonEmitterInfoUpload;

};

struct ParticleRenderResource
{
    std::string            _name;
    ComPtr<ID3D12Resource> _simulationOutput;
    ComPtr<ID3D12Resource> _ribbonSimulationOutput;
    ComPtr<ID3D12Resource> _mvpConstant;
};

struct ParticleSceneResource
{
    std::string                             _name;
    ComPtr<ID3D12GraphicsCommandList>       _commandList;
    ComPtr<ID3D12CommandAllocator>          _commandAllocator;
    std::shared_ptr<ParticleUpdateResource> _updateResource;
    std::unique_ptr<ParticleRenderResource> _renderResource;
};