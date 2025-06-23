// vs_fullscreen.hlsl

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VSOutput vs_main(uint vertexID : SV_VertexID)
{
    float2 pos[3] =
    {
        float2(-1, -1),
        float2(-1, 3),
        float2(3, -1)
    };

    float2 uv[3] =
    {
        float2(0, 0),
        float2(0, 2),
        float2(2, 0)
    };

    VSOutput output;
    output.position = float4(pos[vertexID], 0, 1);
    output.uv = uv[vertexID];
    return output;
}
