#pragma once
#define MAX_PARTICLE 1000000
// 초기값이 있는 멤버 변수 선언 매크로
#define PROP_DECL_INIT(type, varName, initValue)                                                                       \
protected:                                                                                                             \
    type varName = initValue;

// 기존 getter/setter 매크로는 그대로 사용
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

// 초기값이 있는 property 전체 매크로
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
    RIBBON
}; 

enum class VelocityScaleType
{
    LINEAR,
    POINT,
    CONE,
    CUSTOM
};

struct ribbonIndex
{
    UINT  index = -1;
    float ratio = 0;
};


struct ParticleUpdateResource
{
    std::string _name;

    std::vector<class ParticleEffect*>     _sceneEffects;

    std::vector<class Particle>           _totalParticles;
    std::vector<EmitterInfo>              _emitterMatrix;
    std::vector<Texture*>                 _activeEmitterAlbedos;
    UINT                                  _totalCount = 0;

    std::vector<class Particle>           _ribbonTotalParticles;
    std::vector<EmitterInfo>              _ribbonEmitterMatrix;
    std::vector<Texture*>                 _ribbonActiveEmitterAlbedos;
    std::vector<std::vector<ribbonIndex>> _ribbonIndices;
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
    std::string                       _name;
    ComPtr<ID3D12GraphicsCommandList> _commandList;
    ComPtr<ID3D12CommandAllocator>    _commandAllocator;
    ParticleUpdateResource*           _updateResource;
    ParticleRenderResource*           _renderResource;
};

