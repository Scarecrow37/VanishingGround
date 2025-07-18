#include "CommonData.hlsli"
#include "Function.hlsli"

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

StructuredBuffer<uint> vertex_buffer_id: register(t1); //chs
StructuredBuffer<uint> index_buffer_id : register(t2); //chs
StructuredBuffer<Material> material : register(t3); //chs//-> texture id.??=
StructuredBuffer<uint> meshInstanceID : register(t4);
TextureCube evnTexture : register(t5); //chs
StructuredBuffer<RayVertex> Vertices[MAX_MESH] : register(t6);
StructuredBuffer<uint> Indices[MAX_MESH] : register(t2006);
Texture2D textures[] : register(t4006); //chs

struct RayPayload
{
    float4 color;
    uint recursionDepth;
};

struct ShadowPayload
{
    bool hit;
};

float3 CalculateAttribeNoraml(in BuiltInTriangleIntersectionAttributes attribs,uint normalId,uint indexID,uint vertexID)
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
    float3 normalMapSample = textures[normalId].SampleLevel(samLinear_wrap, hitUV, 0).xyz;
    
    float3 worldNormal = normalize(mul((float3x3) ObjectToWorld3x4(), localN));
    float3 worldTangent = normalize(mul((float3x3) ObjectToWorld3x4(), localT));
    float3 worldBitangent = normalize(mul((float3x3) ObjectToWorld3x4(), localB));
    float3 normal = CalculateNormal(normalMapSample, worldTangent, worldBitangent, worldNormal);
    
    return normal;
}

bool TraceShadow(float3 origin,float3 dir,float maxT)
{
    ShadowPayload sp;
    sp.hit = false;
    
    RayDesc sray;
    sray.Origin = origin + dir * 1e-3; // self‑shadow 방지
    sray.Direction = dir;
    sray.TMin = 0.01;
    sray.TMax = maxT;

    // SBTable slot 1 = shadow miss / any‑hit
    TraceRay(RtScene,
             RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH,
             0xFF, 1, 0, 1, sray, sp);

    return sp.hit;
}

[shader("raygeneration")]
void RayGen()
{
    //uint2 launchIndex = DispatchRaysIndex().xy;
    //float2 dims = float2(DispatchRaysDimensions().xy);
    //float2 d = (((launchIndex.xy + 0.5f) / dims.xy) * 2.f - 1.f);

    //RayDesc ray;
    //ray.Origin = mul(cameraData.ViewInverse, float4(0, 0, 0, 1));
    //float4 target = mul(cameraData.ProjectionInverse, float4(d.x, -d.y, 1, 1));
    //ray.Direction = mul(cameraData.ViewInverse, float4(target.xyz, 0));
    //ray.TMin = 0.001;
    //ray.TMax = 100000;
    
    //RayPayload payload;
    //payload.recursionDepth = 0;
    //TraceRay(RtScene,
    //0, 0xFF, 0, 0, 0, ray, payload
    //);
    //Output[launchIndex.xy] = LinearToGammaSpace(payload.color.rgb);
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
    ray.TMax = 10000;

    RayPayload payload;
    payload.recursionDepth = 0;

    TraceRay(RtScene,
             RAY_FLAG_CULL_BACK_FACING_TRIANGLES, 0xFF,
             0, 1, 0, // hitGroup/miss/callable
             ray, payload);

    Output[launchIndex.xy] = float4(payload.color.rgb, 1.f);
}

[shader("miss")]
void Miss(inout RayPayload payload)
{
    float3 dir = normalize(WorldRayDirection());
    float3 sky = evnTexture.SampleLevel(samLinear_wrap, dir,0).rgb;
    sky = saturate(sky);
    sky = GammaToLinearSpace(sky)*10.f;
    payload.color = float4(sky, 1.f);
}

static const uint MAX_RECURSION_DEPTH = 1;

[shader("closesthit")]
void ClosestHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attribs)
{
    uint instanceID = InstanceID();
    //if (InstanceID()==6)
    //    payload.color = 1;
    //else
    //    payload.color = 0;
    
    uint staticMeshInstanceID = meshInstanceID[instanceID];
    uint diffuseID = material[staticMeshInstanceID].ID[DIFFUSE];
    uint normalID = material[staticMeshInstanceID].ID[NORMAL];
    uint ORMID = material[staticMeshInstanceID].ID[ORM];
    uint emissiveID = material[staticMeshInstanceID].ID[EMISSIVE];
    
    uint vertexID = vertex_buffer_id[instanceID];
    uint indexID = index_buffer_id[instanceID];
    
    float3 hitPosition = HitWorldPosition();
    float3 normal = normalize(CalculateAttribeNoraml(attribs, normalID, indexID, vertexID));
    float3 view = normalize(WorldRayOrigin() - hitPosition);
    uint baseIndex = PrimitiveIndex() * 3;
    uint3 indices = uint3(Indices[indexID][baseIndex], 
                          Indices[indexID][baseIndex + 1], 
                          Indices[indexID][baseIndex + 2]);

    float2 uv[3] =
    {
        Vertices[vertexID][indices[0]].uv[0],
        Vertices[vertexID][indices[1]].uv[0],
        Vertices[vertexID][indices[2]].uv[0]
    };
    float2 hitUV = HitAttribute2(uv, attribs);
    float3 albedo = textures[diffuseID].SampleLevel(samLinear_wrap, hitUV, 0).rgb;
    albedo = GammaToLinearSpace(albedo);
    float3 orm = textures[ORMID].SampleLevel(samLinear_wrap, hitUV, 0).rgb;
    float3 emissive = textures[emissiveID].SampleLevel(samLinear_wrap, hitUV, 0).rgb;
    float ao = orm.r;
    float rough = orm.g;
    float metal = orm.b;
    
    float3 directLighting = 0;
    
    // Directional
    for (uint i = 0; i < numLight.Directional; ++i)
    {
        DirectionalLight Ld = lightData.Directional[i];
        float3 L = normalize(-Ld.Direction);
        directLighting += CalculateDirectional(Ld, normal, view, albedo, metal, rough);
        //if (TraceShadow(hitPosition, L, 10000) == false)
    }

    // Point
    for (uint j = 0; j < numLight.Point; ++j)
    {
        PointLight Lp = lightData.Point[j];
        float3 toL = Lp.Position - hitPosition;
        float dist = length(toL);
        float3 L = toL / dist;
        directLighting += CalculatePoint(Lp, normal, view, albedo, metal, rough, hitPosition);
        //if (TraceShadow(hitPosition, L, dist - 0.01) == false)
    }

    // Spot
    for (uint k = 0; k < numLight.Spot; ++k)
    {
        SpotLight Ls = lightData.Spot[k];
        float3 toL = Ls.Position - hitPosition;
        float dist = length(toL);
        float3 L = toL / dist;
        directLighting += CalculateSpot(Ls, normal, view, albedo, metal, rough, hitPosition);
        //if (TraceShadow(hitPosition, L, dist - 0.01) == false)
    }
    /* 환경광 / IBL  */
    float3 envDiffuse = evnTexture.SampleLevel(samLinear_wrap, normal, 0).rgb;
    envDiffuse = saturate(envDiffuse);
    envDiffuse = GammaToLinearSpace(envDiffuse) * 10;
    float3 iblalbedo = albedo * rough;
    float3 ambientLighting = iblalbedo * envDiffuse;

    /* 반사(거울) – FresnelSchlick 사용 */
    float3 reflectionLighting = 0.0;
    if (payload.recursionDepth< MAX_RECURSION_DEPTH)
    {
        float3 reflectionDirection = reflect(-view, normal);
        
        RayDesc reflectionRay;
        reflectionRay.Origin = hitPosition; //+reflectionDirection * Epsilon;
        reflectionRay.Direction = reflectionDirection;
        reflectionRay.TMin = 0.01;
        reflectionRay.TMax = 10000;

        RayPayload reflectionPayload;
        reflectionPayload.recursionDepth = payload.recursionDepth+ 1;

        TraceRay(RtScene,
                 RAY_FLAG_CULL_BACK_FACING_TRIANGLES, // Flags
                 0xFF, // Instance mask
                 0, 1, 0, // SBT	record indices (Hit / Miss / Callable)
                 reflectionRay, reflectionPayload);
 
        /* helper 함수 ‘FresnelSchlick’ 적용 */
        float3 baseReflectance =
            lerp(float3(Fdielectric, Fdielectric, Fdielectric), // 0.04 기본값
                 albedo,metal);

        float3 fresnelFactor =
            FresnelSchlick(saturate(dot(normal, view)), baseReflectance);

        reflectionLighting = reflectionPayload.color.rgb * fresnelFactor;
        //if (TraceShadow(hitPosition, L, 10000) == false)
    }

    /* 최종 색 결과 ------------------------------------------------------- */
    float3 finalcolor =
          emissive +
          ambientLighting + // 환경광 
          directLighting + // 직접광  
          reflectionLighting; // 반사광 
    
    payload.color = float4(finalcolor, 1.f);
}