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
    Matrix FinalMatrix;
    Vector4   Color;
    Vector4   FrameInfo;
    int      EmitterIndex;
    //Vector3   paddings;
};

struct EmitterInfo
{
    Matrix WorldMatrix;
    Vector4 dragPoint;
    Vector4 dragForce;
    Vector4 vortexForce;
    Vector4 startScale;
    Vector4 endScale;
    Vector4 startColor;
    Vector4 endColor;
    Vector4 lifetime;

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



