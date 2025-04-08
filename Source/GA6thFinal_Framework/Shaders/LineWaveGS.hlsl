struct VS_INPUT
{
    float4 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD;
};

cbuffer ViewProjection
{
    matrix vp;
    matrix shadowVP;
}

cbuffer CameraDesc : register(b1)
{
    matrix viewprojInv;
    float3 cameraPosition;
}

[maxvertexcount(4)]
void main(line VS_INPUT input[2], inout TriangleStream<VS_OUTPUT> triStream)
{
    const float lineWidth = 1.f;
    float3 p0 = input[0].position.xyz;
    float3 p1 = input[1].position.xyz;

    // 카메라 방향을 고려한 화면상의 선 방향 벡터 계산
    float3 viewDir = normalize(cameraPosition - (p0 + p1) * 0.5f);
    float3 lineDir = normalize(p1 - p0);
    float3 normal = normalize(cross(viewDir, lineDir)) * lineWidth;

    VS_OUTPUT quad[4];

    quad[0].position = mul(float4(p0 + normal, 1.0f), vp);
    quad[1].position = mul(float4(p0 - normal, 1.0f), vp);
    quad[2].position = mul(float4(p1 + normal, 1.0f), vp);
    quad[3].position = mul(float4(p1 - normal, 1.0f), vp);

    quad[0].uv = float2(0, 0);
    quad[1].uv = float2(0, 1);
    quad[2].uv = float2(1, 0);
    quad[3].uv = float2(1, 1);

    triStream.Append(quad[0]);
    triStream.Append(quad[1]);
    triStream.Append(quad[2]);
    triStream.Append(quad[3]);
    triStream.RestartStrip();
}

//#define SEGMENTS 16
//[maxvertexcount(SEGMENTS * 6)]
//void main(line VS_INPUT input[2], inout TriangleStream<VS_OUTPUT> triStream)
//{
//    float radius = 1.f;
    
//    float3 start = input[0].position.xyz;
//    float3 end = input[1].position.xyz;
    
//    float3 direction = normalize(end - start);
//    float3 up = float3(0.0f, 1.0f, 0.0f);
    
//    if (abs(dot(direction, up)) > 0.99f)
//        up = float3(1.0f, 0.0f, 0.0f);
    
//    float3 right = normalize(cross(direction, up));
//    up = cross(right, direction);

//    float angleStep = 2.0f * 3.14159f / SEGMENTS;

//    for (int i = 0; i < SEGMENTS; ++i)
//    {
//        float angle = i * angleStep;
//        float nextAngle = (i + 1) * angleStep;

//        float3 offset = cos(angle) * right + sin(angle) * up;
//        float3 nextOffset = cos(nextAngle) * right + sin(nextAngle) * up;

//        float3 pos1 = start + offset * radius;
//        float3 pos2 = end + offset * radius;
//        float3 nextPos1 = start + nextOffset * radius;
//        float3 nextPos2 = end + nextOffset * radius;

//        // UV 좌표
//        float u = float(i) / SEGMENTS;
//        float nextU = float(i + 1) / SEGMENTS;
//        float v1 = 0.0f;
//        float v2 = 1.0f;

//        VS_OUTPUT v1_out;
//        v1_out.position = mul(float4(pos1, 1.0f), vp);
//        v1_out.uv = float2(u, v1);

//        VS_OUTPUT v2_out;
//        v2_out.position = mul(float4(pos2, 1.0f), vp);
//        v2_out.uv = float2(u, v2);

//        VS_OUTPUT v3_out;
//        v3_out.position = mul(float4(nextPos1, 1.0f), vp);
//        v3_out.uv = float2(nextU, v1);

//        VS_OUTPUT v4_out;
//        v4_out.position = mul(float4(pos2, 1.0f), vp);
//        v4_out.uv = float2(u, v2);

//        VS_OUTPUT v5_out;
//        v5_out.position = mul(float4(nextPos1, 1.0f), vp);
//        v5_out.uv = float2(nextU, v1);

//        VS_OUTPUT v6_out;
//        v6_out.position = mul(float4(nextPos2, 1.0f), vp);
//        v6_out.uv = float2(nextU, v2);

//        // 삼각형 두 개 추가
//        triStream.Append(v1_out);
//        triStream.Append(v2_out);
//        triStream.Append(v3_out);

//        triStream.Append(v4_out);
//        triStream.Append(v5_out);
//        triStream.Append(v6_out);
//    }
//}