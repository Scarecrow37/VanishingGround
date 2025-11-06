struct ParticleInput
{
    float4   Position;
    float4   Frameinfo; // 
    float3   Axis;
    float    Age;
    float3   Velocity;
    float    Mass;
    int      EmitterIndex;
    float3   SpriteRotation;
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

// --- 기본 축 회전 ---
float3x3 RotX(float rx)
{
    float s = sin(rx), c = cos(rx);
    return float3x3(1, 0, 0, 0, c, -s, 0, s, c);
}
float3x3 RotY(float ry)
{
    float s = sin(ry), c = cos(ry);
    return float3x3(c, 0, s, 0, 1, 0, -s, 0, c);
}
float3x3 RotZ(float rz)
{
    float s = sin(rz), c = cos(rz);
    return float3x3(c, -s, 0, s, c, 0, 0, 0, 1);
}
float3x3 CreateFromEulerXYZ(float3 eulerXYZ)
{
    float pitch = eulerXYZ.x; // X
    float yaw = eulerXYZ.y; // Y
    float roll = eulerXYZ.z; // Z

    return mul(RotY(yaw), mul(RotX(pitch), RotZ(roll)));
}

// 4x4가 필요할 때
float4x4 To4x4(float3x3 R)
{
    return float4x4(
        R[0][0], R[0][1], R[0][2], 0,
        R[1][0], R[1][1], R[1][2], 0,
        R[2][0], R[2][1], R[2][2], 0,
        0, 0, 0, 1
    );
}

float4x4 CreateRotationMatrix(float3 eulerXYZ)
{
    return To4x4(CreateFromEulerXYZ(eulerXYZ));
}

float3 ExtractScale(float4x4 oriented)
{
    float3 axisX = oriented[0].xyz;
    float3 axisY = oriented[1].xyz;
    float3 axisZ = oriented[2].xyz;

    float3 scale = float3(length(axisX), length(axisY), length(axisZ));

    float3 safeScale = max(scale, float3(1e-6f, 1e-6f, 1e-6f));
    float3x3 rotation = float3x3(
          axisX / safeScale.x,
          axisY / safeScale.y,
          axisZ / safeScale.z
      );
    if (determinant(rotation) < 0.0f)
    {
        scale.x *= -1.0f; // 필요하다면 다른 축을 선택해도 됨
    }
    return scale;
}

float4x4 ExtractScaleMatrix(float4x4 oriented, float wComponent = 1.0f)
{
    float3 scale = ExtractScale(oriented);
    return CreateScaleMatrix(float4(scale, wComponent));
}

static const float4x4 IdentityMatrix = 
float4x4(
1, 0, 0, 0, 
0, 1, 0, 0, 
0, 0, 1, 0, 
0, 0, 0, 1);