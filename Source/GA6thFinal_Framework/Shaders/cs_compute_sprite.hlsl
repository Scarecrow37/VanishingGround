#include "Compute.hlsli"

StructuredBuffer<ParticleInput> ParticleInputBuffer : register(t0);
StructuredBuffer<EmitterInfo> EmitterInfoBuffer : register(t1);
RWStructuredBuffer<ParticleOutput> ParticleOutputBuffer : register(u0);


cbuffer MVP : register(b0)
{
    float4x4 ViewMatrix;
    float4x4 ViewInvMatrix;
    float4x4 ProjMatrix;
    float4 CameraPos;
    float deltaTime;
}

[numthreads(32, 1, 1)]
void cs_main(uint3 DTid : SV_DispatchThreadID)
{
 // 현재 처리할 파티클 인덱스
    uint idx = DTid.x;
    
    // 입력 데이터 가져오기
    ParticleInput input = ParticleInputBuffer[idx];
    EmitterInfo emitter = EmitterInfoBuffer[input.emitterIndex];
    
    // 1. 위치 업데이트
    float3 acceleration = float3(0, -9.8, 0) * input.mass;
    input.position.xyz += input.velocity * deltaTime + acceleration * deltaTime;
    
    // 2. 에미터 월드 변환 적용
    float4 worldPos = mul(float4(input.position.xyz, 1.0), emitter.WorldMatrix);
    
    // 3. 빌보딩 행렬 계산
    float4x4 billboardMat = CalculateBillboardMatrix(
        worldPos.xyz, ViewInvMatrix
            );
    
    // 4. 스케일 적용
    float4x4 scaleMat = CreateScaleMatrix(
        lerp(float4(input.startEndScale.xy, 1, 1), float4(input.startEndScale.zw, 1, 1), input.age / input.lifetime)
    );
    
    // 5. 최종 행렬 계산
    ParticleOutput output;
    output.FinalMatrix = mul(mul(mul(scaleMat, billboardMat), ViewMatrix), ProjMatrix);
    
    // 6. 색상 보간
    output.Color = lerp(input.startColor, input.endColor, input.age / input.lifetime);
    
    // 7. 프레임 애니메이션
    output.FrameInfo = UpdateAnimation(input.frameinfo, deltaTime);
    
    // 결과 저장
    ParticleOutputBuffer[idx] = output;
}