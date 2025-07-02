#include "CommonData.hlsli"

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
};

VSOutput vs_main(uint id : SV_VertexID)
{
    float2 quad[4] =
    {
        float2(-1, -1),
        float2(-1, 1),
        float2(1, -1),
        float2(1, 1)
    };

    float2 uv = quad[id];

    float gridExtent = 2000.0f; // 큰 범위로 설정

    // 카메라 위치 기준 평면상의 월드 좌표 계산
    float3 worldPos;
    worldPos.xz = cameraData.Position.xz + uv * gridExtent;
    worldPos.y = 0;

    VSOutput o;
    o.position = mul(float4(worldPos, 1.0), cameraData.View);
    o.position = mul(o.position, cameraData.Projection);
    o.worldPos = worldPos;
    
    return o;
}