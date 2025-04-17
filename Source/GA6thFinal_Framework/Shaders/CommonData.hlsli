struct CameraData
{
    matrix View;
    matrix Projection;
    matrix ViewInverse;
    matrix ProjectionInverse;
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