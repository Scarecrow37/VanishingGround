#include "CommonData.hlsli"

#define PARALLAX_HEIGHT_SCALE_DIVISOR 200

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
    float4 worldPosition : TEXCOORD1;
    
    nointerpolation uint4 materialID : TEXCOORD2;
    nointerpolation uint customDepth : TEXCOORD3;
    nointerpolation float alpha : TEXCOORD4;
};

struct PSOutput
{
    float4 baseColor : SV_Target0;
    float4 normal : SV_Target1;
    float4 orm : SV_Target2;
    float4 emissive : SV_Target3;
    float depth : SV_Target4;
    uint customDepth : SV_Target5;
};

#define DIFFUSE   0
#define NORMAL    1
#define ORM       2
#define EMISSIVE  3

Texture2D textures[];
ConstantBuffer<GbufferData> bit32_2_gbufferData;

// ---------------------- 유틸 ----------------------

static const float EPSILON_POM = 1e-5f;

// TBN 직교정규화: tangent를 normal에 대해 그램-슈미트, bitangent는 cross로 재구축
void OrthonormalizeTBN(inout float3 T, inout float3 B, inout float3 N)
{
    N = normalize(N);
    T = normalize(T - N * dot(T, N));
    B = normalize(cross(N, T));
}

// 원래 코드의 POM 누적 (SampleGrad 유지)
float2 CalculatePOMUVOffset(float2 parallaxOffset, float2 uv, int numSteps, uint ORMID)
{
    float currentHeight = 0.0f;
    float stepSize = 1.0f / (float) numSteps;
    float prevHeight = 1.0f;
    float currentBound = 1.0f;

    float2 texOffsetPerStep = stepSize * parallaxOffset;
    float2 texCurrentOffset = uv;

    float2 pt1 = 0;
    float2 pt2 = 0;

    float2 dx = ddx(uv);
    float2 dy = ddy(uv);

    [loop]
    for (int stepIndex = 0; stepIndex < numSteps; ++stepIndex)
    {
        texCurrentOffset -= texOffsetPerStep;

        // 심을 넘을 수 있으므로 래핑 안전을 위해 수동 frac 적용
        float2 sampleUV = frac(texCurrentOffset);

        currentHeight = textures[ORMID].SampleGrad(samLinear_wrap, sampleUV, dx, dy).a;
        currentBound -= stepSize;

        if (currentHeight > currentBound)
        {
            pt1 = float2(currentBound, currentHeight);
            pt2 = float2(currentBound + stepSize, prevHeight);
            break;
        }
        prevHeight = currentHeight;
    }

    // 선형 보간
    float delta2 = pt2.x - pt2.y;
    float delta1 = pt1.x - pt1.y;
    float diff = delta2 - delta1;

    float parallaxAmount = (abs(diff) < EPSILON_POM) ? 0.0f : (pt1.x * delta2 - pt2.x * delta1) / diff;
    float2 vParallaxOffset = parallaxOffset * (1.0 - parallaxAmount);

    // 심을 넘어가는 경우를 대비해 frac로 고정
    return frac(uv - vParallaxOffset);
}

// 시선각 기반 스텝 수
int GetPOMRayStepsCount(float3 worldPos, float3 N)
{
    int minLayers = 8;
    int maxLayers = 32;
    float ndotv = saturate(dot(normalize(cameraData.Position.xyz - worldPos), normalize(N)));
    return (int) lerp(maxLayers, minLayers, ndotv);
}

// ---------------------- 메인 ----------------------

PSOutput WriteGuBuffer(PSInput input)
{
    PSOutput output = (PSOutput) 0;
    
    float mipOffset = bit32_2_gbufferData.MipBias;
   
    uint diffuseID = input.materialID[DIFFUSE];
    float alpha = textures[diffuseID].SampleBias(samLinear_wrap, input.uv, mipOffset).a;
    alpha *= input.alpha;
    clip(alpha - CUTOFF); // Masked Alpha Test
    
    uint normalID = input.materialID[NORMAL];
    uint ORMID = input.materialID[ORM];
    uint emissiveID = input.materialID[EMISSIVE];

    // TBN 직교정규화 (심/왜곡 구간 안정화에 중요)
    float3 T = input.tangent;
    float3 B = input.biTangent;
    float3 N = input.normal;
    OrthonormalizeTBN(T, B, N);
    float3x3 TBN = float3x3(T, B, N);

    float2 parallaxUV = input.uv;

    // 높이맵 미리 샘플 (원래 UV로): height==1은 평면 가정인 듯하니 동일 조건 유지
    float height = textures[ORMID].SampleBias(samLinear_wrap, input.uv, mipOffset).a;

    if (height < 1.0f)
    {
        // viewDir in TS
        float3 viewDirWS = cameraData.Position.xyz - input.worldPosition.xyz;
        float3 viewDirTS = mul(TBN, viewDirWS);

        // 카메라가 뒤에서 보거나 z가 너무 작으면 POM 비활성
        if (viewDirTS.z > 1e-4f)
        {
            // 시선각 기반 강도/스텝
            int stepCount = GetPOMRayStepsCount(input.worldPosition.xyz, N);
            float ndotv = saturate(dot(normalize(viewDirWS), N));

            // 안전한 오프셋 공식: (xy / z) * scale
            float2 dirTS = normalize(viewDirTS.xy);
            float scale = (bit32_2_gbufferData.HeightScale) / PARALLAX_HEIGHT_SCALE_DIVISOR * (1.0f - ndotv); // 시선이 비스듬할수록 강해짐
            float2 parallaxOffset = (dirTS / max(viewDirTS.z, 1e-4f)) * scale;

            // seam 폭주 방지를 위한 클램프(필요 시 수치 조정)
            parallaxOffset = clamp(parallaxOffset, -0.25f, 0.25f);

            parallaxUV = CalculatePOMUVOffset(parallaxOffset, input.uv, stepCount, ORMID);
        }
        else
        {
            // 뒤에서 보는 경우: POM 패스
            parallaxUV = frac(parallaxUV);
        }
    }
    else
    {
        parallaxUV = frac(parallaxUV);
    }

    // 0. baseColor
    output.baseColor = textures[diffuseID].SampleBias(samLinear_wrap, parallaxUV, mipOffset);

    // 1. normal (TS→WS)
    float3 normalTS = textures[normalID].SampleBias(samLinear_wrap, parallaxUV, mipOffset).xyz;
    normalTS = normalize(normalTS * 2.0f - 1.0f);
    float3 normalWS = normalize(mul(normalTS, TBN));
    output.normal = float4(normalWS, 1.0f);

    // 2. ORM
    float4 ormSample = textures[ORMID].SampleBias(samLinear_wrap, parallaxUV, mipOffset);
    output.orm = float4(ormSample.r, ormSample.g, ormSample.b, 1.0f);

    // 3. emissive
    output.emissive = textures[emissiveID].SampleBias(samLinear_wrap, parallaxUV, mipOffset);

    // 4. depth (clip-space z 그대로 저장하던 기존 로직 유지)
    // 필요 시: LinearizeDepth(input.position.z, near, far)로 교체 가능
    output.depth = input.position.z;

    // 5. customDepth
    output.customDepth = input.customDepth;

    return output;
}

PSOutput ps_main(PSInput input)
{
    return WriteGuBuffer(input);
}

