#include "Compute.hlsli"
StructuredBuffer<ParticleInput> ParticleInputBuffer : register(t0);
StructuredBuffer<EmitterInfo> EmitterInfoBuffer : register(t1);
RWStructuredBuffer<ParticleOutput> ParticleOutputBuffer : register(u0);
ConstantBuffer<MVP> mvp : register(b0);





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
    float ratio = saturate(input.Age / emitter.Particlelifetime);
    
    
    float dragCoefficient = emitter.DragForce.z;
    float decay = exp(-dragCoefficient * input.Age);

    
    // Get the useWorldSpace flag (1.0f for true, 0.0f for false)
    float useWorldSpace = emitter.Particlelifetime.y;

    // Calculate the rotation center. It's the emitter's world position if in world space, otherwise it's (0,0,0).
    float3 emitterCenter = emitter.WorldMatrix[3].xyz * useWorldSpace;

    // Calculate the vector from the rotation center to the particle.
    float3 r = input.Position.xyz - emitterCenter;

    // Get the base vortex axis and normalize it.
    float3 vortexAxis = emitter.VortexForce.xyz;
    float3 localAxisDir = normalize(vortexAxis);

    // Transform the axis to world space to account for the emitter's rotation.
    float3 worldAxisDir = mul(localAxisDir, (float3x3) emitter.WorldMatrix);

    // Select the correct axis direction using lerp to avoid branching.
    // If useWorldSpace is 1.0, worldAxisDir is chosen. If 0.0, localAxisDir is chosen.
    float3 axisDir = lerp(localAxisDir, worldAxisDir, useWorldSpace);

    // Calculate vortex strength and velocity.
    float vortexAttenuation = emitter.VortexForce.w;
    float vortexStrength = length(vortexAxis);
    float strength = vortexStrength / (1.0 + vortexAttenuation * length(r));
    float3 vortexVelocity = cross(axisDir, r) * strength;
    float3 vortexDisplacement = vortexVelocity * input.Age;
    float3 posAfterVortex = input.Position.xyz + vortexDisplacement;
    float3 dragPos = (input.Velocity / max(dragCoefficient, 0.01f)) * (1 - decay);
    
    input.Position.xyz += dragPos + vortexDisplacement;
    

        // 6. 색상 보간
    float3 outputColor = lerp(emitter.StartColor.rgb, emitter.EndColor.rgb, ratio);
    float outputOpacity = lerp(emitter.StartColor.a, emitter.EndColor.a, ratio);
    output.Color = float4(outputColor, outputOpacity);
   
    // 4. 스케일 적용
    float4 currentScale = lerp(float4(emitter.StartScale.xy, 1, 1), float4(emitter.EndScale.xy, 1, 1), ratio);
    float4x4 scaleMat = CreateScaleMatrix(currentScale);
    
    float4 worldPos = mul(float4(input.Position.xyz, 1.0), emitter.WorldMatrix);
    worldPos.xyz += gravityOffset * input.Age;
    float4 viewPos = mul(worldPos, mvp.ViewMatrix);
    
    output.Position = worldPos;

        
        
    float4 ribbonnormal = lerp(emitter.StartNormal, emitter.EndNormal, ratio);
    float4x4 ribbonworld = lerp(input.InitialMatrix, emitter.WorldMatrix, useWorldSpace);
    ribbonnormal.w = 0;
    ribbonnormal = mul(ribbonnormal, ribbonworld);
    emitter.RibbonVector.w = 0;
    float4 ribbonDir = mul(emitter.RibbonVector, ribbonworld);
    
    output.Paddings = cross(ribbonDir.xyz, ribbonnormal.xyz);

    output.EmitterIndex = input.EmitterIndex;
    
    
    float4x4 translationMat = float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
    worldPos.x, worldPos.y, worldPos.z, 1);

    output.FinalMatrix = scaleMat;
    output.FinalMatrix = mul(output.FinalMatrix, translationMat);
    output.FinalMatrix = mul(output.FinalMatrix, mvp.ViewMatrix);
    output.FinalMatrix = mul(output.FinalMatrix, mvp.ProjMatrix);

    output.FrameInfo = float4(currentScale.x, currentScale.y, ratio, 0);
    
    
    
    ParticleOutputBuffer[idx*2] = output;
    ParticleOutputBuffer[idx*2+1] = output;
}