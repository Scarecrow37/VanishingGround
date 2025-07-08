#include "Compute.hlsli"

StructuredBuffer<ParticleInput> ParticleInputBuffer : register(t0);
StructuredBuffer<EmitterInfo> EmitterInfoBuffer : register(t1);
RWStructuredBuffer<ParticleOutput> ParticleOutputBuffer : register(u0);


struct MVP
{
    float4x4 ViewMatrix;
    float4x4 ViewRotInvMatrix;
    float4x4 ProjMatrix;
    float4 CameraPos;
    float deltaTime;
    float4 pad1;
    float4 pad2;
    float3 pad3;
};

ConstantBuffer<MVP> mvp : register(b0);





[numthreads(64, 1, 1)]
void cs_main(uint3 DTid : SV_DispatchThreadID)
{
 // 현재 처리할 파티클 인덱스
    uint idx = DTid.x;
    
    // 입력 데이터 가져오기
    ParticleInput input = ParticleInputBuffer[idx];
    EmitterInfo emitter = EmitterInfoBuffer[input.emitterIndex];
    ParticleOutput output;
    
    float3 acceleration = float3(0, -9.8, 0) * input.mass;
    float3 gravityOffset = acceleration * input.age;



    float ratio = saturate(input.age / input.lifetime);
    float dragCoefficient = emitter.dragforce.z/2;

    float3 newPos = input.velocity * input.age;
    float newposlen = length(newPos);
    float3 dragforce = dragCoefficient * newPos * ratio * ratio * dragCoefficient;
    float dragforcelen = length(dragforce);
    float3 normaldrag = normalize(dragforce);
    newPos = newPos - normaldrag * min(dragforcelen, newposlen);
    input.position.xyz += newPos;
    
    

    
    

        // 6. 색상 보간
    float3 outputColor = lerp(input.startColor, input.endColor, ratio);
    float outputOpacity = lerp(input.startopacity, input.endopacity, ratio);
    output.Color = float4(outputColor, outputOpacity);
   
    // 4. 스케일 적용
    float4x4 scaleMat = CreateScaleMatrix(
        lerp(float4(input.startScale.xy, 1, 1), float4(input.endScale.xy, 1, 1), ratio)
    );
    
    float4 worldPos = mul(float4(input.position.xyz, 1.0), emitter.WorldMatrix);
    worldPos.xyz += gravityOffset;
    float4 viewPos = mul(worldPos, mvp.ViewMatrix);
    
    output.position = viewPos;
    output.paddings = (float3) 0;

    output.EmitterIndex = input.emitterIndex;
    
    
    float4x4 translationMat = float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
    worldPos.x, worldPos.y, worldPos.z, 1
);

    output.FinalMatrix = scaleMat;
    output.FinalMatrix = mul(output.FinalMatrix, mvp.ViewRotInvMatrix);
    //output.FinalMatrix = mul(output.FinalMatrix, worldinvrot);
    output.FinalMatrix = mul(output.FinalMatrix, translationMat);
    output.FinalMatrix = mul(output.FinalMatrix, mvp.ViewMatrix);
    output.FinalMatrix = mul(output.FinalMatrix, mvp.ProjMatrix);
    
    

    // 7. 프레임 애니메이션
    output.FrameInfo = UpdateAnimation(input.frameinfo, mvp.deltaTime);
    // 결과 저장
    ParticleOutputBuffer[idx] = output;
}