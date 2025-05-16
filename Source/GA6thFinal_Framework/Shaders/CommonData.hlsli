#define PI 3.141592654
#define Epsilon 0.00001
#define Fdielectric  0.04

struct DirectionalLight
{
    float4 direction;
    float4 strength;
};

struct PointLight
{
    float3 position;
    float fallOffEnd;
    float4 strength;
};

struct SpotLight
{
    float3 position;
    float fallOffEnd;
    float4 direction;
    float spotInnerCone;
    float spotOuterCone;
    float2 padding;
    float4 strength;
};

struct NumLight
{
    uint Directional;
    uint Point;
    uint Spot;
    uint padding;
};

struct CameraData
{
    matrix View;
    matrix Projection;
    matrix ViewInverse;
    matrix ProjectionInverse;
    float4 Position;
};

struct ObjectData
{
    matrix World;
};

struct Object
{
    uint ID;
};

ConstantBuffer<CameraData> cameraData : register(b0);
ConstantBuffer<Object> bit32_object   : register(b1);

#define object bit32_object

SamplerState samPoint_wrap : register(s0);
SamplerState samPoint_clamp : register(s1);
SamplerState samLinear_wrap : register(s2);
SamplerState samLinear_clamp : register(s3);
SamplerState samAnistropic_wrap : register(s4);
SamplerState samAnistropic_clamp : register(s5);