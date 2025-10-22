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
    Vector4 Position; // ribbon -> normal
    Matrix  FinalMatrix;
    Vector4 Color;
    Vector4 FrameInfo;
    int     EmitterIndex;
    Vector3 Paddings;
};

struct EmitterInfo
{
    Matrix  WorldMatrix;
    Matrix  OrientedWorldMatrix;
    Vector4 DragPoint;
    Vector4 DragForce;
    Vector4 VortexForce;
    Vector4 StartScale;
    Vector4 EndScale;
    Vector4 StartColor;
    Vector4 EndColor;
    Vector4 Lifetime; // x: particle lifetime, y: useWorldSpace (1.0f for true, 0.0f for false)
    Vector4 StartNormal;
    Vector4 EndNormal;
    Vector4 RibbonVector;
};

struct __declspec(align(16)) MVPConstants
{
    Matrix  ViewMatrix;
    Matrix  ViewRotInvMatrix;
    Matrix  ProjMatrix;
    Vector4 CameraPos;
    float   DeltaTime; // 4바이트

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
    UINT  Index = -1;
    float Ratio = 0;
};

struct ParticleUpdateResource
{
    std::string Name;

    std::vector<std::unique_ptr<class ParticleEffect>> SceneEffects;

    std::vector<class Particle>           TotalParticles;
    std::vector<EmitterInfo>              EmitterMatrix;
    std::vector<UINT>                     ActiveEmitterAlbedos;
    UINT                                  TotalCount = 0;
    std::vector<class Particle>           RibbonTotalParticles;
    std::vector<EmitterInfo>              RibbonEmitterMatrix;
    std::vector<UINT>                     RibbonActiveEmitterAlbedos;
    std::vector<std::vector<RibbonIndex>> RibbonIndices;
    UINT                                  RibbonTotalCount = 0;
    std::vector<std::vector<RibbonIndex>> RibbonIndicesRawBackup;


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
    std::string Name;

    // 기존 출력(그대로 유지)
    ComPtr<ID3D12Resource> SimulationOutput;       // ParticleOutput[]
    ComPtr<ID3D12Resource> RibbonSimulationOutput; // ParticleOutput[]
    ComPtr<ID3D12Resource> MvpConstant;            // MVPConstants

    // 세분화(인터폴레이션) 결과: VS/PS가 읽을 최종 ParticleOutput[]
    ComPtr<ID3D12Resource> RibbonInterpolatedOutput; // UAV/SRV

    // CS 입력용 보조 테이블들 (SRV) + 업로드 버퍼
    //  - 에미터별 "원본 리본 포인트 인덱스"를 이어붙인 배열
    ComPtr<ID3D12Resource> RibbonConcatIndices;       // SRV: uint[]
    ComPtr<ID3D12Resource> RibbonConcatIndicesUpload; // UPLOAD

    //  - ConcatIdx 내에서 각 에미터의 시작 오프셋 (size = emitterCount + 1; 마지막은 가드)
    ComPtr<ID3D12Resource> RibbonEmitOffsets;       // SRV: uint[]
    ComPtr<ID3D12Resource> RibbonEmitOffsetsUpload; // UPLOAD

    //  - 세분화 "출력" 버퍼에서 각 에미터의 시작 오프셋 (prefix 결과)
    ComPtr<ID3D12Resource> RibbonEmitStarts;       // SRV: uint[]
    ComPtr<ID3D12Resource> RibbonEmitStartsUpload; // UPLOAD

    // 세분화 출력 최대 수용량
    UINT RibbonInterpolatedCapacity = 0;
};

struct ParticleSceneResource
{
    std::string                             Name;
    ComPtr<ID3D12GraphicsCommandList>       CommandList;
    ComPtr<ID3D12CommandAllocator>          CommandAllocator;
    std::shared_ptr<ParticleUpdateResource> UpdateParticleResource;
    std::unique_ptr<ParticleRenderResource> RenderParticleResource;
};