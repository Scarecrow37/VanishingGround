struct VS_INPUT
{
    float4 position : POSITION;
    float2 waveData : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
};

//cbuffer ViewProjection
//{
//    matrix vp;
//    matrix shadowVP;
//}

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    //output.position = mul(input.position, vp);
    output.position = input.position;
    
    return output;
}