#include "Compute.hlsli"
StructuredBuffer<ParticleInput> ParticleInputBuffer : register(t0);
StructuredBuffer<EmitterInfo> EmitterInfoBuffer : register(t1);
RWStructuredBuffer<ParticleOutput> ParticleOutputBuffer : register(u0);
ConstantBuffer<MVP> mvp : register(b0);

[numthreads(64, 1, 1)]
void cs_main(uint3 DTid : SV_DispatchThreadID)
{
    uint idx = DTid.x;

    // 입력
    ParticleInput input = ParticleInputBuffer[idx];
    EmitterInfo emitter = EmitterInfoBuffer[input.EmitterIndex];
    ParticleOutput output;

    // 기본 물리/색/스케일 계산 (기존 그대로)
    float3 acceleration = float3(0, -9.8, 0) * input.Mass;
    float3 gravityOffset = acceleration * input.Age;
    float ratio = saturate(input.Age / emitter.Particlelifetime.x);
    
    
    float dragCoefficient = emitter.DragForce.z;
    float decay = exp(-dragCoefficient * input.Age);

    float useWorldSpace = emitter.Particlelifetime.y;
    float useWorldScale = clamp(emitter.DragForce.y, 0.f, 1.f);

    float3 emitterCenter = emitter.WorldMatrix[3].xyz * useWorldSpace;

    float3 r = input.Position.xyz - emitterCenter;

    float3 vortexAxis = emitter.VortexForce.xyz;
    float3 localAxisDir = normalize(vortexAxis);

    float3 worldAxisDir = mul(localAxisDir, (float3x3) emitter.WorldMatrix);

    float3 axisDir = lerp(localAxisDir, worldAxisDir, useWorldSpace);

    float vortexAttenuation = emitter.VortexForce.w;
    float vortexStrength = length(vortexAxis);
    float strength = vortexStrength / (1.0 + vortexAttenuation * length(r));
    float3 vortexVelocity = cross(axisDir, r) * strength;
    float3 vortexDisplacement = vortexVelocity * input.Age;

    float3 dragPos = (input.Velocity / max(dragCoefficient, 0.01f)) * (1 - decay);
    input.Position.xyz += dragPos + vortexDisplacement;

    // 색
    float3 outputColor = lerp(emitter.StartColor.rgb, emitter.EndColor.rgb, ratio);
    float outputOpacity = lerp(emitter.StartColor.a, emitter.EndColor.a, ratio);
    output.Color = float4(outputColor, outputOpacity);
   
    // 4. 스케일 적용
    float4 currentScale = lerp(float4(emitter.StartScale.xy, 1, 1), float4(emitter.EndScale.xy, 1, 1), ratio);
    float4x4 localScaleMat = CreateScaleMatrix(currentScale);
    float4x4 worldScaleMat = ExtractScaleMatrix(emitter.OrientedWorldMatrix);
    localScaleMat = mul(localScaleMat, lerp(IdentityMatrix, worldScaleMat, useWorldScale));

    float ribbonWidth = localScaleMat[0][0];
    float ribbonThickness = localScaleMat[1][1];
    output.FrameInfo = float4(ribbonWidth, ribbonThickness, ratio, 0);

    float4 worldPos = mul(float4(input.Position.xyz, 1.0), emitter.WorldMatrix);
    worldPos.xyz += gravityOffset * input.Age;
    output.Position = worldPos; // 일단 center로 세팅(아래서 Top/Bottom으로 덮어씀)

    // 리본 방향 벡터(패딩스에 저장하는 사이드 벡터)
    float4 ribbonnormal = lerp(emitter.StartNormal, emitter.EndNormal, ratio);
    float4x4 ribbonworld = lerp(input.InitialMatrix, emitter.WorldMatrix, useWorldSpace);
    ribbonnormal.w = 0;
    ribbonnormal = mul(ribbonnormal, ribbonworld);

    emitter.RibbonVector.w = 0;
    float4 ribbonDir = mul(emitter.RibbonVector, ribbonworld);

    // 화면에서 위/아래로 벌릴 사이드 벡터 (VS 참고)
    float3 side = cross(ribbonDir.xyz, ribbonnormal.xyz);

    // 안전 정규화(0 길이 가드)
    float sideLen2 = dot(side, side);
    side = (sideLen2 < 1e-12f) ? float3(0, 1, 0) : normalize(side);

    output.Paddings = side; // 사이드 벡터는 그대로 보관(파이프라인 일관성)
    output.EmitterIndex = input.EmitterIndex;

    // 최종 변환 매트릭스(프로젝션까지)
    float4x4 translationMat = float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        worldPos.x, worldPos.y, worldPos.z, 1);

    output.FinalMatrix = localScaleMat;
    output.FinalMatrix = mul(output.FinalMatrix, translationMat);
    output.FinalMatrix = mul(output.FinalMatrix, mvp.ViewMatrix);
    output.FinalMatrix = mul(output.FinalMatrix, mvp.ProjMatrix);


    //output.FrameInfo = float4(currentScale.x, currentScale.y, ratio, 0);
    
    ParticleOutputBuffer[idx*2] = output;
    ParticleOutputBuffer[idx*2+1] = output;
}