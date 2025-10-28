 #include "CommonData.hlsli"

struct GSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 worldPosition : TEXCOORD1;
    nointerpolation uint4 materialID : TEXCOORD2;
};

struct GSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 worldPosition : TEXCOORD1;
    float3 lightPosition : TEXCOORD2;
    float farPlane : TEXCOORD3;
    nointerpolation uint4 materialID : TEXCOORD4;
    uint viewport : SV_ViewportArrayIndex;
};

ConstantBuffer<PointLightShadowData> pointLightShadowData;

[maxvertexcount(18)] // 3 vertices * 6 faces
void gs_main(triangle GSInput input[3], inout TriangleStream<GSOutput> triStream)
{
    [unroll]
    for (uint faceIndex = 0; faceIndex < 6; ++faceIndex)
    {
        GSOutput output;
        output.viewport = faceIndex;
        output.lightPosition = pointLightShadowData.LightPosition;
        output.farPlane = pointLightShadowData.FarPlane;
        [unroll]
        for (uint i = 0; i < 3; ++i)
        {
            float4 worldPos = float4(input[i].worldPosition, 1.0);
            output.position = mul(worldPos, pointLightShadowData.ViewProjection[faceIndex]);
            output.uv = input[i].uv;
            output.worldPosition = input[i].worldPosition;
            output.materialID = input[i].materialID;

            triStream.Append(output);
        }
        triStream.RestartStrip();
    }
}