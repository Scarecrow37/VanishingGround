#pragma once

struct Vertex
{
	Vector4 Position;
	Vector3 Normal;
	Vector3 Tangent;
	Vector3 BiTangent;
	Vector2 UV;
};

struct StaticMeshVertex
{
	Vector4 Position;
	Vector3 Normal;
	Vector3 Tangent;
	Vector3 BiTangent;
	Vector2 UV;
};

struct SkeletalMeshVertex : public Vertex
{
	UINT BlendIndices[4]{};
	float BlendWeights[4]{};
};

struct ObjectData
{
	XMMATRIX World;
};

struct MaterialData
{
	UINT ID[4];
};

struct CameraData
{	
	XMMATRIX View;
	XMMATRIX Projection;
	XMMATRIX ViewInverse;
	XMMATRIX ProejctionInverse;
    Vector4  Position{0.f,0.f,-5.f,1.f};
};

struct NumLight
{
    UINT Directional;
    UINT Point;
    UINT Spot;
    UINT Padding; 
};