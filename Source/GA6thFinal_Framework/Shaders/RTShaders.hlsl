#include "CommonData.hlsli"
#include "Function.hlsli"
static const float EPSILON_POM = 1e-5f;
#define PARALLAX_HEIGHT_SCALE_DIVISOR 200

#define DIFFUSE 0
#define NORMAL 1
#define ORM 2
#define EMISSIVE 3
// mesh갯수나 texture 갯수는 unbounded이기 때문에 이런식으로 사용
// descriptor heap size == 2000
#define MAX_MESH 2000
// Retrieve hit world position.
float3 HitWorldPosition()
{
    return WorldRayOrigin() + RayTCurrent() * WorldRayDirection();
}

float3 HitAttribute3(float3 vertexAttribute[3], BuiltInTriangleIntersectionAttributes attr)
{
    return vertexAttribute[0] +
        attr.barycentrics.x * (vertexAttribute[1] - vertexAttribute[0]) +
        attr.barycentrics.y * (vertexAttribute[2] - vertexAttribute[0]);
}

float2 HitAttribute2(float2 vertexAttribute[3], BuiltInTriangleIntersectionAttributes attr)
{
    return vertexAttribute[0] +
        attr.barycentrics.x * (vertexAttribute[1] - vertexAttribute[0]) +
        attr.barycentrics.y * (vertexAttribute[2] - vertexAttribute[0]);
}

float3 CalculateNormal(float3 sampledNormal, float3 tangent, float3 bitangent, float3 normal)
{
    sampledNormal = normalize(sampledNormal * 2.0 - 1.0);
    float3x3 TBN = float3x3(tangent, bitangent, normal);
    return normalize(mul(sampledNormal, TBN));
}


struct RayVertex
{
    float4 position;
    float3 normal;
    float3 tangent;
    float3 bitangent;
    float2 uv[2];
};

struct Material
{
    uint ID[4];
};

RaytracingAccelerationStructure RtScene : register(t0); //chs
RWTexture2D<float4> Output : register(u0);

StructuredBuffer<uint> vertex_buffer_id : register(t1); //chs
StructuredBuffer<uint> index_buffer_id : register(t2); //chs
StructuredBuffer<InstanceData> meshInstanceData : register(t3); //chs
ConstantBuffer<GbufferData> bit32_2_gbufferData : register(b4);
TextureCube evnTexture : register(t5); //chs
TextureCube irradianceTexture : register(t6); //chs
TextureCube prefilteredMap : register(t7); //chs
Texture2D brdfLUT : register(t8); //chs

StructuredBuffer<RayVertex> Vertices[MAX_MESH] : register(t9);
StructuredBuffer<uint> Indices[MAX_MESH] : register(t2009);
Texture2D textures[] : register(t4009); //chs

struct RayPayload
{
    float4 color;
    uint recursionDepth;
};

struct ShadowPayload
{
    bool hit;
};

int GetPOMRayStepsCount(float3 worldPos, float3 N, float3 cameraPos)
{
    int minLayers = 8;
    int maxLayers = 32;
    float ndotv = saturate(dot(normalize(cameraPos - worldPos), normalize(N)));
    return (int) lerp(maxLayers, minLayers, ndotv);
}

float2 CalculatePOMUVOffset(float2 parallaxOffset, float2 uv, int numSteps, uint ORMID, float mipLevel)
{
    float currentHeight = 0.0f;
    float stepSize = 1.0f / (float) numSteps;
    float prevHeight = 1.0f;
    float currentBound = 1.0f;

    float2 texOffsetPerStep = stepSize * parallaxOffset;
    float2 texCurrentOffset = uv;

    float2 pt1 = 0;
    float2 pt2 = 0;

         [loop]
    for (int stepIndex = 0; stepIndex < numSteps; ++stepIndex)
    {
        texCurrentOffset -= texOffsetPerStep;
        float2 sampleUV = frac(texCurrentOffset);

             // 레이트레이싱에서는 SampleLevel 사용
        currentHeight = textures[ORMID].SampleLevel(samLinear_wrap, sampleUV, mipLevel).a;
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

    return frac(uv - vParallaxOffset);
}

float3 CalculateAttribeNoraml(in BuiltInTriangleIntersectionAttributes attribs, uint normalId, uint indexID, uint vertexID, float mipLevel)
{
    //mesh 단위 blas에서의 삼각형 번호.
    uint baseIndex = PrimitiveIndex() * 3;
    uint3 indices = uint3(
    Indices[indexID][baseIndex],
    Indices[indexID][baseIndex + 1],
    Indices[indexID][baseIndex + 2]
);
    float3 vN[3] =
    {
        Vertices[vertexID][indices[0]].normal,
        Vertices[vertexID][indices[1]].normal,
        Vertices[vertexID][indices[2]].normal
    };

    float3 vT[3] =
    {
        Vertices[vertexID][indices[0]].tangent,
        Vertices[vertexID][indices[1]].tangent,
        Vertices[vertexID][indices[2]].tangent
    };

    float3 vB[3] =
    {
        Vertices[vertexID][indices[0]].bitangent,
        Vertices[vertexID][indices[1]].bitangent,
        Vertices[vertexID][indices[2]].bitangent
    };
    
    float2 uv[3] =
    {
        Vertices[vertexID][indices[0]].uv[0],
        Vertices[vertexID][indices[1]].uv[0],
        Vertices[vertexID][indices[2]].uv[0]
    };
    float2 hitUV = HitAttribute2(uv, attribs);
    
    // 바리센터릭 보간으로 히트 지점 속성 구하기 ────────────────────────
    float3 localN = normalize(HitAttribute3(vN, attribs));
    float3 localT = normalize(HitAttribute3(vT, attribs));
    float3 localB = normalize(HitAttribute3(vB, attribs));
    float3 normalMapSample = textures[normalId].SampleLevel(samAnistropic_wrap, hitUV, mipLevel).xyz;
    
    float3 worldNormal = normalize(mul((float3x3) ObjectToWorld3x4(), localN));
    float3 worldTangent = normalize(mul((float3x3) ObjectToWorld3x4(), localT));
    float3 worldBitangent = normalize(mul((float3x3) ObjectToWorld3x4(), localB));
    float3 normal = CalculateNormal(normalMapSample, worldTangent, worldBitangent, worldNormal);
    
    return normal;
}

bool TraceShadow(float3 origin, float3 dir, float maxT)
{
    ShadowPayload sp;
    sp.hit = true;
    
    RayDesc sray;
    sray.Origin = origin + dir * 1e-3; // self‑shadow 방지
    sray.Direction = dir;
    sray.TMin = 0.01;
    sray.TMax = maxT;

    // SBTable slot 1 = shadow miss
    TraceRay(RtScene,
             RAY_FLAG_CULL_BACK_FACING_TRIANGLES,
             0xFF, 1, 0, 1, sray, sp);

    return sp.hit;
}

[shader("raygeneration")]
void RayGen()
{
    uint2 launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (((launchIndex + 0.5f) / dims) * 2.f - 1.f);
    float4x4 viewI = cameraData.ViewInverse;
    float4x4 projI = cameraData.ProjectionInverse;

    RayDesc ray;
    ray.Origin = mul(viewI, float4(0, 0, 0, 1)).xyz;
    float4 target = mul(projI, float4(d.x, -d.y, 1, 1));
    target /= target.w;
    ray.Direction = normalize(mul(viewI, float4(target.xyz, 0))).xyz;
    ray.TMin = 0.01;
    ray.TMax = 2000;

    RayPayload payload;
    payload.recursionDepth = 0;

    TraceRay(RtScene,
             RAY_FLAG_NONE, 0xFF,
             0, 0, 0, // hitGroup/miss/callable
             ray, payload);

    Output[launchIndex.xy] = float4(payload.color.rgb, 1.f);
}

[shader("miss")]
void Miss(inout RayPayload payload)
{
    float3 dir = normalize(WorldRayDirection());
    float3 sky = evnTexture.SampleLevel(samLinear_wrap, dir, 0).rgb;
    sky = saturate(sky);
    sky = GammaToLinearSpace(sky) * 10.f;
    payload.color = float4(sky, 1.f);
}

[shader("miss")]
void ShadowMiss(inout ShadowPayload payload)
{
    payload.hit = false; // 그림자 미스는 hit가 false
}

static const uint MAX_RECURSION_DEPTH = 1;

[shader("closesthit")]
void ClosestHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attribs)
{
    uint instanceID = InstanceID();
    InstanceData instData = meshInstanceData[instanceID];
    uint diffuseID = instData.MaterialID[DIFFUSE];
    uint normalID = instData.MaterialID[NORMAL];
    uint ORMID = instData.MaterialID[ORM];
    uint emissiveID = instData.MaterialID[EMISSIVE];

    uint vertexID = vertex_buffer_id[instanceID];
    uint indexID = index_buffer_id[instanceID];

    float3 hitPosition = HitWorldPosition();
    float distanceToCamera = length(cameraData.Position.xyz - hitPosition);
    float mipLevel = ComputeDynamicMipLevel(distanceToCamera, 12);

    uint baseIndex = PrimitiveIndex() * 3;
    uint3 indices = uint3(Indices[indexID][baseIndex],
                               Indices[indexID][baseIndex + 1],
                               Indices[indexID][baseIndex + 2]);

    // Barycentric 보간으로 버텍스 속성 가져오기
    float3 vN[3] =
    {
        Vertices[vertexID][indices[0]].normal,
             Vertices[vertexID][indices[1]].normal,
             Vertices[vertexID][indices[2]].normal
    };
    float3 vT[3] =
    {
        Vertices[vertexID][indices[0]].tangent,
             Vertices[vertexID][indices[1]].tangent,
             Vertices[vertexID][indices[2]].tangent
    };
    float3 vB[3] =
    {
        Vertices[vertexID][indices[0]].bitangent,
             Vertices[vertexID][indices[1]].bitangent,
             Vertices[vertexID][indices[2]].bitangent
    };
    float2 uv[3] =
    {
        Vertices[vertexID][indices[0]].uv[0],
             Vertices[vertexID][indices[1]].uv[0],
             Vertices[vertexID][indices[2]].uv[0]
    };

    float3 localN = normalize(HitAttribute3(vN, attribs));
    float3 localT = normalize(HitAttribute3(vT, attribs));
    float3 localB = normalize(HitAttribute3(vB, attribs));
    float2 hitUV = HitAttribute2(uv, attribs);
    
    float3 worldN = normalize(mul((float3x3) ObjectToWorld3x4(), localN));
    float3 worldT = normalize(mul((float3x3) ObjectToWorld3x4(), localT));
    float3 worldB = normalize(mul((float3x3) ObjectToWorld3x4(), localB));
    
    OrthonormalizeTBN(worldT, worldB, worldN);
    float3x3 TBN = float3x3(worldT, worldB, worldN);
    
    float2 parallaxUV = hitUV;
    
    float height = textures[ORMID].SampleLevel(samLinear_wrap, hitUV, mipLevel).a;

    if (height < 1.0f)
    {
        float3 viewDirWS = cameraData.Position.xyz - hitPosition;
        float3 viewDirTS = mul(TBN, viewDirWS);

        if (viewDirTS.z > 1e-4f)
        {
            int stepCount = GetPOMRayStepsCount(hitPosition, worldN, cameraData.Position.xyz);
            float ndotv = saturate(dot(normalize(viewDirWS), worldN));

            float2 dirTS = normalize(viewDirTS.xy);
            float scale = (bit32_2_gbufferData.HeightScale) / PARALLAX_HEIGHT_SCALE_DIVISOR * (1.0f - ndotv);
            float2 parallaxOffset = (dirTS / max(viewDirTS.z, 1e-4f)) * scale;
            
            parallaxOffset = clamp(parallaxOffset, -0.25f, 0.25f);

            parallaxUV = CalculatePOMUVOffset(parallaxOffset, hitUV, stepCount, ORMID, mipLevel);
        }
        else
        {
            parallaxUV = frac(parallaxUV);
        }
    }
    else
    {
        parallaxUV = frac(parallaxUV);
    }
    
    float3 normalMapSample = textures[normalID].SampleLevel(samAnistropic_wrap, parallaxUV, mipLevel).xyz;
    float3 normalTS = normalize(normalMapSample * 2.0f - 1.0f);
    float3 normal = normalize(mul(normalTS, TBN));

    float3 view = normalize(WorldRayOrigin() - hitPosition);

    float3 emissive = textures[emissiveID].SampleLevel(samAnistropic_wrap, parallaxUV, mipLevel).rgb;
    float3 albedo = textures[diffuseID].SampleLevel(samAnistropic_wrap, parallaxUV, mipLevel).rgb;
    albedo = GammaToLinearSpace(albedo);

    float3 orm = textures[ORMID].SampleLevel(samAnistropic_wrap, parallaxUV, mipLevel).rgb;
    float ao = orm.r;
    float rough = orm.g;
    float metal = orm.b;
    
    float3 directLighting = 0;
    float3 ambientLighting = 0;
    // Directional
    for (uint i = 0; i < bit32_4_numLight.Directional; ++i)
    {
        DirectionalLight Ld = lightData.Directional[i];
        float3 L = normalize(-Ld.Direction);
        
        /* 환경광 / IBL  */
        ambientLighting += CalculateIBL(normal, view, irradianceTexture, prefilteredMap, brdfLUT, albedo, rough, metal) * Ld.Ambient;
        
        if (TraceShadow(hitPosition, L, 2000) == false)
            directLighting += CalculateDirectional(Ld, normal, view, albedo, metal, rough);
    }

    // Point
    for (uint j = 0; j < bit32_4_numLight.Point; ++j)
    {
        PointLight Lp = lightData.Point[j];
        float3 toL = Lp.Position - hitPosition;
        float dist = length(toL);
        float3 L = toL / dist;
        if (TraceShadow(hitPosition, L, dist - 0.01) == false)
            directLighting += CalculatePoint(Lp, normal, view, albedo, metal, rough, hitPosition);
    }
    
    // Shadow Point
    for (uint k = 0; k < bit32_4_numLight.ShadowPoint; ++k)
    {
        PointLight Lp = lightData.Point[k];
        float3 toL = Lp.Position - hitPosition;
        float dist = length(toL);
        float3 L = toL / dist;
        if (TraceShadow(hitPosition, L, dist - 0.01) == false)
            directLighting += CalculatePoint(Lp, normal, view, albedo, metal, rough, hitPosition);
    }

    // Spot
    for (uint l = 0; l < bit32_4_numLight.Spot; ++l)
    {
        SpotLight Ls = lightData.Spot[l];
        float3 toL = Ls.Position - hitPosition;
        float dist = length(toL);
        float3 L = toL / dist;
        if (TraceShadow(hitPosition, L, dist - 0.01) == false)
            directLighting += CalculateSpot(Ls, normal, view, albedo, metal, rough, hitPosition);
    }

    /* 반사(거울) – FresnelSchlick 사용 */
    float3 reflectionLighting = 0.0;
    if (payload.recursionDepth < MAX_RECURSION_DEPTH)
    {
        float3 reflectionDirection = normalize(reflect(-view, normal));
        
        RayDesc reflectionRay;
        reflectionRay.Origin = hitPosition + reflectionDirection * Epsilon;
        reflectionRay.Direction = reflectionDirection;
        reflectionRay.TMin = 0.01;
        reflectionRay.TMax = 2000;
       
        RayPayload reflectionPayload;
        reflectionPayload.recursionDepth = payload.recursionDepth + 1;
        reflectionPayload.color = float4(0, 0, 0, 1); // 초기화
        
        TraceRay(RtScene,
                 RAY_FLAG_NONE, // Flags
                 0xFF, // Instance mask
                 0, 1, 0, // SBT	record indices (Hit / Miss / Callable)
                 reflectionRay, reflectionPayload);
 
        /* helper 함수 ‘FresnelSchlick’ 적용 */
        float3 baseReflectance =
            lerp(float3(Fdielectric, Fdielectric, Fdielectric), // 0.04 기본값
                 albedo, metal);

        float3 fresnelFactor =
            FresnelSchlick(saturate(dot(reflectionDirection, view)), baseReflectance);
        reflectionLighting = reflectionPayload.color.rgb * fresnelFactor;
    }
    float reflectivity = lerp(0.04, 1.0, metal); // 금속 여부에 따라
    float reflectionWeight = reflectivity * (1.0 - rough * rough); // 조절식
    reflectionLighting *= reflectionWeight;
    /* 최종 색 결과 ------------------------------------------------------- */
    float3 finalcolor =
          emissive +
          ambientLighting + // 환경광 
          directLighting + // 직접광  
          reflectionLighting; // 반사광 
    
    payload.color = float4(finalcolor, 1.f);
}
