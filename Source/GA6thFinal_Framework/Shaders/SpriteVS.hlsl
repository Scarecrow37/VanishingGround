struct VS_INPUT
{
    float4 position : POSITION;
    float2 uv       : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position      : SV_POSITION;
    float2 uv            : TEXCOORD0;
};

cbuffer CameraData
{
    matrix view;
    matrix projection;
    float3 cameraPosition;
};

cbuffer World
{
    matrix world;
};

#ifdef Billboard
float4 ComputeBillboard(float3 position)
{
    float3 lookAtDir = normalize(cameraPosition - position);
    float3 upDir = float3(0.0f, 1.0f, 0.0f);
    float3 rightDir = cross(upDir, lookAtDir);
    
    matrix rotationMatrix = matrix(rightDir.x, upDir.x, lookAtDir.x, 0.0f,
                                   rightDir.y, upDir.y, lookAtDir.y, 0.0f,
                                   rightDir.z, upDir.z, lookAtDir.z, 0.0f,
                                   0.0f, 0.0f, 0.0f, 1.0f);
    
    return mul(float4(position, 1), rotationMatrix);
}
#endif

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;    

    float4 position = float4(input.position.xy, 1, 1);
    output.position = mul(position, world);
    
#ifdef D3D
#ifdef Billboard
    output.position = ComputeBillboard(output.position.xyz);
#else
    output.position = mul(output.position, view);
#endif
#endif
    
    output.position = mul(output.position, projection);
    output.uv = input.uv;
    
    return output;
}