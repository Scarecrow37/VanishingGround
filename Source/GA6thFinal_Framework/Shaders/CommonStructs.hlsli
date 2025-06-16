#define MAX_DIRECTIONAL_LIGHT 4
#define MAX_POINT_LIGHT 32
#define MAX_SPOT_LIGHT 16

struct DirectionalLight
{
    float3 Color;
    float Intensity;
    float3 Direction;
    float Padding1;
    float3 Ambient;
    float Padding2;
    float4 Padding3;
};

struct PointLight
{
    float3 Color;
    float Intensity;
    float3 Position;
    float Range;
    float3 Attenuation;
    float Padding1;
    float4 Padding2;
};

struct SpotLight
{
    float3 Color;
    float Intensity;
    float3 Position;
    float Range;
    float3 Direction;
    float InnerCone;
    float3 Attenuation;
    float OuterCone;
};

struct NumLight
{
    uint Directional;
    uint Point;
    uint Spot;
};

struct CameraData
{
    matrix View;
    matrix Projection;
    matrix ViewInverse;
    matrix ProjectionInverse;
    float4 Position;
};

struct LightData
{
    DirectionalLight Directional[MAX_DIRECTIONAL_LIGHT];
    PointLight Point[MAX_POINT_LIGHT];
    SpotLight Spot[MAX_SPOT_LIGHT];
};

struct ObjectData
{
    uint ID;
    uint Offset;
};