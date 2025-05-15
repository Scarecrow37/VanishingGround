struct ParticleInput
{
    float4 position;
    float4 startPosition;
    float4 color;
    float4 startColor;
    float4 endColor;
    float4 frameinfo; // duration, elapsedTime, currentIndex, isLoop
    float3 scale;
    float lifetime;
    float3 axis;
    float age;
    float3 velocity;
    int emitterIndex;
    float4 startEndScale;
    float mass;
};



struct ParticleOutput
{
    float4x4 FinalMatrix;
    float4 Color;
    float4 FrameInfo;
    
};



struct EmitterInfo
{
    float4x4 WorldMatrix;
    float drag;
    
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

// 애니메이션 업데이트 함수
float4 UpdateAnimation(float4 frameInfo, float deltaTime)
{
    float duration = frameInfo.x;
    frameInfo.y += deltaTime;
    uint currentFrame = frameInfo.z;

    if (frameInfo.y >= duration)
    {

        currentFrame = min(currentFrame + 1, (uint) frameInfo.w - 1);
        frameInfo.y -= duration;
       
    }

    return float4(duration, elapsed, currentFrame, frameInfo.w);
}
float4x4 CreateScaleMatrix(float4 scale)
{
    return float4x4(
    scale.x, 0, 0, 0,
    0, scale.y, 0, 0,
    0, 0, scale.z, 0,
    0, 0, 0, 1
);
}