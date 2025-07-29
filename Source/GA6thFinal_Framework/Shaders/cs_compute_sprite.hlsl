#include "Compute.hlsli"

StructuredBuffer<ParticleInput> ParticleInputBuffer : register(t0);
StructuredBuffer<EmitterInfo> EmitterInfoBuffer : register(t1);
RWStructuredBuffer<ParticleOutput> ParticleOutputBuffer : register(u0);

ConstantBuffer<MVP> mvp : register(b0);




float4x4 GetBillBoardRotationMatrix(float3 particleAxis, float3 particleWorldPos)
{
    if (length(particleAxis) < 0.001)
    {
        return mvp.ViewRotInvMatrix;
    }
    else
    {
        float3 viewVec = normalize(mvp.CameraPos.xyz - particleWorldPos);
        float3 rightVec = normalize(cross(viewVec, particleAxis));
        float3 upVec = normalize(cross(particleAxis, rightVec));
        float4x4 axialbilboard = float4x4(
               rightVec.x, rightVec.y, rightVec.z, 0,
               upVec.x, upVec.y, upVec.z, 0,
               particleAxis.x, particleAxis.y, particleAxis.z, 0,
               0, 0, 0, 1
           );
        return axialbilboard;
    }
}

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
    float ratio = saturate(input.age / emitter.particlelifetime);
    
    
    float dragCoefficient = emitter.dragforce.z;
    float decay = exp(-dragCoefficient * input.age);

    
    // Get the useWorldSpace flag (1.0f for true, 0.0f for false)
    float useWorldSpace = emitter.particlelifetime.y;

    // Calculate the rotation center. It's the emitter's world position if in world space, otherwise it's (0,0,0).
    float3 emitterCenter = emitter.WorldMatrix[3].xyz * useWorldSpace;

    // Calculate the vector from the rotation center to the particle.
    float3 r = input.position.xyz - emitterCenter;

    // Get the base vortex axis and normalize it.
    float3 vortexAxis = emitter.vortexForce.xyz;
    float3 localAxisDir = normalize(vortexAxis);

    // Transform the axis to world space to account for the emitter's rotation.
    float3 worldAxisDir = mul(localAxisDir, (float3x3)emitter.WorldMatrix);

    // Select the correct axis direction using lerp to avoid branching.
    // If useWorldSpace is 1.0, worldAxisDir is chosen. If 0.0, localAxisDir is chosen.
    float3 axisDir = lerp(localAxisDir, worldAxisDir, useWorldSpace);

    // Calculate vortex strength and velocity.
    float vortexAttenuation = emitter.vortexForce.w;
    float vortexStrength = length(vortexAxis);
    float strength = vortexStrength / (1.0 + vortexAttenuation * length(r));
    float3 vortexVelocity = cross(axisDir, r) * strength;
    float3 vortexDisplacement = vortexVelocity * input.age;
    float3 posAfterVortex = input.position.xyz + vortexDisplacement;
    float3 dragPos = (input.velocity / max(dragCoefficient, 0.01f)) * (1 - decay);
    
    
    
    float3 finalVelocity = dragPos + vortexDisplacement;
    
    input.position.xyz += finalVelocity;
    
    
    
    
    

        // 6. 색상 보간
    float3 outputColor = lerp(emitter.startColor.rgb, emitter.endColor.rgb, ratio);
    float outputOpacity = lerp(emitter.startColor.a, emitter.endColor.a, ratio);
    output.Color = float4(outputColor, outputOpacity);
   
    // 4. 스케일 적용
    float axisfactor = lerp(1, length(finalVelocity), step(0.001f, length(input.axis)));
    float4 scalefactor = lerp(float4(emitter.startScale.xy, 1, 1), float4(emitter.endScale.xy, 1, 1), ratio) * axisfactor;
    float4x4 scaleMat = CreateScaleMatrix(scalefactor);
    
    float4 worldPos = mul(float4(input.position.xyz, 1.0), emitter.WorldMatrix);
    worldPos.xyz += gravityOffset*input.age;
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
    float4 worldAxis = float4(input.axis, 1);
    worldAxis = mul(worldAxis, emitter.OrientedWorldMatrix);
    
    float4x4 rotation = GetBillBoardRotationMatrix(worldAxis.xyz, worldPos.xyz);
    
    
    
    
    
    output.FinalMatrix = mul(output.FinalMatrix, rotation);
    
    output.FinalMatrix = mul(output.FinalMatrix, translationMat);
    output.FinalMatrix = mul(output.FinalMatrix, mvp.ViewMatrix);
    output.FinalMatrix = mul(output.FinalMatrix, mvp.ProjMatrix);
    
    

    // 7. 프레임 애니메이션
    output.FrameInfo = UpdateAnimation(input.frameinfo, mvp.deltaTime);
    // 결과 저장
    ParticleOutputBuffer[idx] = output;
}