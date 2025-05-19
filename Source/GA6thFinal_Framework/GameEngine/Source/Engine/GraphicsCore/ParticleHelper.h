#pragma once

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
#define UMPARTICLE_PROPERTY(type, varName, FuncName, initValue)                                                   \
    PROP_DECL_INIT(type, varName, initValue)                                                                           \
    PROP_GET(type, varName, FuncName)                                                                                  \
    PROP_SET(type, varName, FuncName)

#define UMPARTICLE_PROPERTY_REF(type, varName, FuncName, initValue)                                               \
    PROP_DECL_INIT(type, varName, initValue)                                                                           \
    PROP_GET_REF(type, varName, FuncName)                                                                              \
    PROP_SET_REF(type, varName, FuncName)


#define BYTEALIGN(value, alignment) (((value) + ((alignment) - 1)) & ~((alignment) - 1))



struct ParticleOutput
{
    Matrix FinalMatrix;
    Vector4   Color;
    Vector4   FrameInfo;
};

struct EmitterInfo
{
    Matrix WorldMatrix;
};

struct MVPConstants
{
    Matrix  ViewMatrix;
    Matrix  ViewInvMatrix;
    Matrix  ProjMatrix;
    Vector4 CameraPos;
    float   deltaTime;
    float   padding[3]; // 16바이트 정렬 유지
};
