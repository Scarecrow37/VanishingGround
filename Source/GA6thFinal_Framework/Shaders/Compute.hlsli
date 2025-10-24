struct ParticleInput
{
    float4   Position;
    float4   Frameinfo; // 
    float3   Axis;
    float    Age;
    float3   Velocity;
    float    Mass;
    int      EmitterIndex;
    float3   Paddings;
    float4x4 InitialMatrix;
};



struct ParticleOutput
{
    float4 Position; //ribbon -> normal
    float4x4 FinalMatrix;
    float4 Color;
    float4 FrameInfo; // ribbon-> x = ribbon width
    int EmitterIndex;
    float3 Paddings;
    
};



struct EmitterInfo
{
    float4x4 WorldMatrix;
    float4x4 OrientedWorldMatrix;
    float4   DragPoint;
    float4   DragForce;
    float4   VortexForce;
    float4   StartScale;
    float4   EndScale;
    float4   StartColor;
    float4   EndColor;
    float4   Particlelifetime;
    float4   StartNormal;
    float4   EndNormal;
    float4   RibbonVector;
};


struct MVP
{
    float4x4 ViewMatrix;
    float4x4 ViewRotInvMatrix;
    float4x4 ProjMatrix;
    float4 CameraPos;
    float DeltaTime;
    float4 Pad1;
    float4 Pad2;
    float3 Pad3;
};

struct TessParams
{
    uint TessFactor;
    uint TotalSegments;
    uint EmitterCount;
    uint _pad1;
};


// 빌보딩 행렬 계산 함수
float4x4 CalculateAxialBillboardMatrix(
    float3 position,
    float3 cameraPos,
    float3 fixedAxis)
{
    float3 toCamera = normalize(cameraPos - position);
    float3 right = normalize(cross(fixedAxis, toCamera));
    float3 forward = normalize(cross(right, fixedAxis));

    return float4x4(
        right.x, right.y, right.z, 0,
        fixedAxis.x, fixedAxis.y, fixedAxis.z, 0,
        forward.x, forward.y, forward.z, 0,
        position.x, position.y, position.z, 1
    );
}

float4x4 CalculateBillboardMatrix(
    float3 position, float4x4 viewInvMatrix)
{
    viewInvMatrix._14_24_34 = 0.f;
    viewInvMatrix._41_42_43 = 0.f;
    viewInvMatrix._44 = 1.f;
    return viewInvMatrix;
}

float4x4 CreateScaleMatrix(float4 scale)
{
    return float4x4(
    scale.x, 0, 0, 0,
    0, scale.y, 0, 0,
    0, 0, scale.z, 0,
    0, 0, 0, 1);
}



// ============================================================
// Catmull–Rom (float 스칼라) 기본 폴리노미얼~이다
// p0,p1,p2,p3는 연속 제어점, t ∈ [0,1]은 구간 [p1,p2]의 로컬 파라미터~이다
// ============================================================
inline float CatmullRom(float t, float p0, float p1, float p2, float p3)
{
    float t2 = t * t;
    float t3 = t2 * t;
    float a0 = -0.5f * p0 + 1.5f * p1 - 1.5f * p2 + 0.5f * p3;
    float a1 = p0 - 2.5f * p1 + 2.0f * p2 - 0.5f * p3;
    float a2 = -0.5f * p0 + 0.5f * p2;
    float a3 = p1;
    return a0 * t3 + a1 * t2 + a2 * t + a3;
}

// ============================================================
// (float3) Catmull–Rom: Vector3 버전 (C++의 Vector3::CatmullRom 대응)~이다
// ============================================================
inline float3 CatmullRom(float u, float3 p0, float3 p1, float3 p2, float3 p3)
{
    return float3(
        CatmullRom(u, p0.x, p1.x, p2.x, p3.x),
        CatmullRom(u, p0.y, p1.y, p2.y, p3.y),
        CatmullRom(u, p0.z, p1.z, p2.z, p3.z)
    );
}


