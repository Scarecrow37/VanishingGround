#include "Compute.hlsli"

StructuredBuffer<ParticleInput> ParticleInputBuffer : register(t0);
StructuredBuffer<EmitterInfo> EmitterInfoBuffer : register(t1);
RWStructuredBuffer<ParticleOutput> ParticleOutputBuffer : register(u0);
ConstantBuffer<MVP> mvp : register(b0);

float4x4 GetBillBoardRotationMatrix(float3 particleAxis, float3 particleWorldPos,float4x4 worldmatrix)
{
    if (length(particleAxis) < 0.001)
    {
        return mvp.ViewRotInvMatrix;
    }
    else
    {
        float4 worldAxis = float4(particleAxis, 1);
        worldAxis = mul(worldAxis, worldmatrix);
        float3 viewVec = normalize(mvp.CameraPos.xyz - particleWorldPos);
        float3 rightVec = normalize(cross(viewVec, worldAxis.xyz));
        float3 upVec = normalize(cross(worldAxis.xyz, rightVec));
        float4x4 axialbilboard = float4x4(
               rightVec.x, rightVec.y, rightVec.z, 0,
               upVec.x, upVec.y, upVec.z, 0,
               worldAxis.x, worldAxis.y, particleAxis.z, 0,
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
    EmitterInfo emitter = EmitterInfoBuffer[input.EmitterIndex];
    ParticleOutput output;
    
    float3 acceleration = float3(0, -9.8, 0) * input.Mass;
    float3 gravityOffset = acceleration * input.Age;
    float ratio = saturate(input.Age / emitter.Particlelifetime.x); // .x를 명시적으로 사용
    float dragCoefficient = emitter.DragForce.z;
    float decay = exp(-dragCoefficient * input.Age);
    float useWorldSpace = emitter.Particlelifetime.y;
    float3 emitterCenter = emitter.WorldMatrix[3].xyz * useWorldSpace;
    float3 r = input.Position.xyz - emitterCenter;
    float3 vortexAxis = emitter.VortexForce.xyz;
    float3 localAxisDir = normalize(vortexAxis);
    float3 worldAxisDir = mul(localAxisDir, (float3x3) emitter.OrientedWorldMatrix);
    float3 axisDir = lerp(localAxisDir, worldAxisDir, useWorldSpace);
    float vortexAttenuation = emitter.VortexForce.w;
    float vortexStrength = length(vortexAxis);
    float strength = vortexStrength / (1.0 + vortexAttenuation * length(r));
    float3 vortexVelocity = cross(axisDir, r) * strength;
    float3 vortexDisplacement = vortexVelocity * input.Age;
    
    float3 dragPos = (input.Velocity / max(dragCoefficient, 0.01f)) * (1 - decay);
    
    float3 totalVel = dragPos + vortexDisplacement;


    float3 outputColor = lerp(emitter.StartColor.rgb, emitter.EndColor.rgb, ratio);
    float outputOpacity = lerp(emitter.StartColor.a, emitter.EndColor.a, ratio);
    output.Color = float4(outputColor, outputOpacity);
   
    float axisfactor = lerp(1, length(totalVel), step(0.001f, length(input.Axis)));
    float4 scalefactor = lerp(emitter.StartScale, emitter.EndScale, ratio) * axisfactor;
    float4x4 scaleMat = CreateScaleMatrix(scalefactor);
    
    float4 worldPos = mul(float4(input.Position.xyz, 1.0), emitter.WorldMatrix);
    float4 finalVelocity = mul(float4(totalVel, 0), emitter.OrientedWorldMatrix);
    
    worldPos.xyz += finalVelocity + gravityOffset * input.Age;
    
    float4 viewPos = mul(worldPos, mvp.ViewMatrix);
    output.Position = viewPos;
    
    output.Paddings = input.Paddings;
    output.EmitterIndex = input.EmitterIndex;
    
    float4x4 translationMat = float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        worldPos.x, worldPos.y, worldPos.z, 1
    );

    float4x4 rotation = GetBillBoardRotationMatrix(input.Axis, worldPos.xyz, emitter.OrientedWorldMatrix);
    
    output.FinalMatrix = mul(scaleMat, rotation);
    output.FinalMatrix = mul(output.FinalMatrix, translationMat);
    output.FinalMatrix = mul(output.FinalMatrix, mvp.ViewMatrix);
    output.FinalMatrix = mul(output.FinalMatrix, mvp.ProjMatrix);
    
    output.FrameInfo = input.Frameinfo;
    
    ParticleOutputBuffer[idx] = output;
}