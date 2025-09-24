#include "CommonData.hlsli"
#include "Function.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    nointerpolation uint instanceID : TEXCOORD1;
};

cbuffer BurnCB : register(b0)
{
    float burnProgress;
    float edgeWidth;
    float scorchWidth;
    float glowIntensity;
    
    float2 noiseTiling;
    float2 noiseScroll;
    float2 distortTiling;
    float2 distortScroll;
    
    float noiseInfluence;
    float distortStrength;
    float t;
}

Texture2D NoiseTex : register(t1);
Texture2D EdgeRampTex : register(t2);

Texture2D textures[];

// d=0에서 1, |d|>=width에서 0
float EdgeMask(float d, float width)
{
    float e = 1.0 - saturate(abs(d) / max(width, 1e-4));
    return pow(saturate(e), 1.5);
}

// 경계 안쪽 그을음
float ScorchMask(float d, float width)
{
    return saturate(1.0 - smoothstep(0.0, max(width, 1e-4), d));
}

float2 FlowUV(float2 uv, float2 tiling, float2 scroll, float t)
{
    return uv * tiling + scroll * t;
}

float4 ps_main(PSInput pin) : SV_Target0
{
    // 노이즈 샘플
    float2 nUV = FlowUV(pin.uv, noiseTiling, noiseScroll, t);
    float n = NoiseTex.Sample(samLinear_clamp, nUV).r;

    // 진행선: 아래(uv.y=0)에서 위(uv.y=1)로
    // 만약 텍스처가 상단이 0이라면 uv.y를 (1-uv.y)로 바꾸세요.
    float d = pin.uv.y + noiseInfluence * (n - 0.5) - burnProgress;

    // 이미 탄 영역 컷아웃
    if (d < 0.0)
    {
        clip(-1);
    }

    float e = EdgeMask(d, edgeWidth); // 가장자리 마스크
    float s = ScorchMask(d, scorchWidth); // 그을음 마스크

    // 가장자리 근처만 UV 왜곡(열기 슈머)
    float2 kUV = FlowUV(pin.uv, distortTiling, distortScroll, t);
    float2 k2 = NoiseTex.Sample(samLinear_clamp, kUV).rg * 2.0 - 1.0;
    float2 uvD = pin.uv + k2 * (distortStrength * e);

    //float4 baseCol = Base.Sample(samLinear_clamp, uvD);
    float4 baseCol = 0;
    
    // 그을음 적용
    float scorchFactor = lerp(1.0, 0.35, s);
    baseCol.rgb *= scorchFactor;

    // 가장자리 발광: 램프 x=e (0~1)
    float3 edgeGlow = EdgeRampTex.Sample(samLinear_clamp, float2(e, 0.5)).rgb * (e * glowIntensity);

    float3 finalRGB = baseCol.rgb + edgeGlow;
    float finalA = baseCol.a;

    return float4(finalRGB, finalA);
}