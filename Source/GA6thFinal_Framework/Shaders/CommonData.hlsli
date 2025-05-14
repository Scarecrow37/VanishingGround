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