#define PI 3.141592654
#define Epsilon 0.00001

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