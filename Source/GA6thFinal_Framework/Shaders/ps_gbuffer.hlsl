#include "CommonData.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
    float4 worldPosition : TEXCOORD1;
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

#define DIFFUSE 0
#define NORMAL 1
#define ORM 2
#define EMISSIVE 3

struct Material
{
    uint ID[4];
};

StructuredBuffer<Material> material;
Texture2D textures[];
ConstantBuffer<ParallaxMappingData> bit32_1_parallaxProperty;
float2 CalculatePOMUVOffset(float2 parallaxOffset, float2 uv, int numSteps, uint ORMID)
{
    float currentHeight = 0.0f;
    float stepSize = 1.0f / (float) numSteps;
    float prevHeight = 1.0f;
    float nextHeight = 0.0f;
    int stepIndex = 0;

    float2 texOffsetPerStep = stepSize * parallaxOffset;
    float2 texCurrentOffset = uv;
    float currentBound = 1.0;
    float parallaxAmount = 0.0;

    float2 pt1 = 0;
    float2 pt2 = 0;

    float2 texOffset2 = 0;
    float2 dx = ddx(uv);
    float2 dy = ddy(uv);
    
    [loop]
    while (stepIndex < numSteps)
    {
        texCurrentOffset -= texOffsetPerStep;
        currentHeight = textures[ORMID].SampleGrad(samLinear_wrap, texCurrentOffset, dx, dy).a;
        currentBound -= stepSize;

        if (currentHeight > currentBound)
        {
            pt1 = float2(currentBound, currentHeight); // point from current height
            pt2 = float2(currentBound + stepSize, prevHeight); // point from previous height

            texOffset2 = texCurrentOffset - texOffsetPerStep;

            stepIndex = numSteps + 1;
        }
        else
        {
            stepIndex++;
            prevHeight = currentHeight;
        }
    }
   
    //linear interpolation
    float delta2 = pt2.x - pt2.y;
    float delta1 = pt1.x - pt1.y;
    float diff = delta2 - delta1;
      
    if (diff == 0.0f)
        parallaxAmount = 0.0f;
    else
        parallaxAmount = (pt1.x * delta2 - pt2.x * delta1) / diff;
   
    float2 vParallaxOffset = parallaxOffset * (1.0 - parallaxAmount);
    return uv - vParallaxOffset;
}
int GetPOMRayStepsCount(float3 worldPos, float3 normal)
{
    int minLayers = 8;
    int maxLayers = 32;
    
    int numLayers = (int) lerp(maxLayers, minLayers, dot(normalize(cameraData.Position.rgb - worldPos), normal));
    return numLayers;
}

float3 CalculateNormal(float3 sampledNormal, float3 tangent, float3 bitangent, float3 normal)
{
    sampledNormal = normalize(sampledNormal * 2.0 - 1.0);
    float3x3 TBN = float3x3(tangent, bitangent, normal);
    return normalize(mul(sampledNormal, TBN));
}

PSOutput WriteGuBuffer(PSInput input)
{
    PSOutput output = (PSOutput) 0;
    uint diffuseID = material[objectData.ID].ID[DIFFUSE];
    uint normalID = material[objectData.ID].ID[NORMAL];
    uint ORMID = material[objectData.ID].ID[ORM];
    uint emissiveID = material[objectData.ID].ID[EMISSIVE];
    
    float2 parallaxUV = input.uv;
    float3x3 TBN = float3x3(input.tangent, input.biTangent, input.normal);
    float height = textures[ORMID].Sample(samLinear_wrap, input.uv).a;
    int stepCount = 0;
    if (height < 1.f)
    {
// parallax mapping start
        float3 viewDirWorld = (cameraData.Position.xyz - input.worldPosition.xyz);
        float3 TangentViewDir = mul(TBN, viewDirWorld);
        float2 parallaxDir = normalize(TangentViewDir.xy);
        float viewDirTSLength = length(TangentViewDir);
        float parallaxLength = sqrt(viewDirTSLength * viewDirTSLength - TangentViewDir.z * TangentViewDir.z) / TangentViewDir.z;
        float2 parallaxOffset = parallaxDir * parallaxLength * bit32_1_parallaxProperty.HeightScale;

        stepCount = GetPOMRayStepsCount(input.worldPosition.xyz, input.normal);
        parallaxUV = CalculatePOMUVOffset(parallaxOffset, input.uv, stepCount, ORMID);
// parallax mapping end
    }

    // 0. baseColor
    output.baseColor = textures[diffuseID].Sample(samLinear_wrap, parallaxUV);
    output.baseColor.rgb += input.worldPosition.xyz * 0.0001f;
    
    // 1. normal
    float3 normal = textures[normalID].Sample(samLinear_wrap, parallaxUV).xyz;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(mul(normal, TBN));
    output.normal = float4(normal, 1.f);
    
    //2. ORM
    float ao = textures[ORMID].Sample(samLinear_wrap, parallaxUV).r;
    float roughness = textures[ORMID].Sample(samLinear_wrap, parallaxUV).g;
    float metallic = textures[ORMID].Sample(samLinear_wrap, parallaxUV).b;
    output.orm = float4(ao, roughness, metallic, 1.f);
    
    //3. emissive
    output.emissive = textures[emissiveID].Sample(samLinear_wrap, parallaxUV);

    //4. depth
    output.depth = input.position.z;
    
    //5. customDepth
    output.customDepth = objectData.CustomDepth;
    return output;
}

PSOutput ps_main(PSInput input)
{
    PSOutput output = (PSOutput) 0;
    output = WriteGuBuffer(input);

    return output;
}
