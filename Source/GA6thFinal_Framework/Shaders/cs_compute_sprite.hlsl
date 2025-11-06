#include "Compute.hlsli"

StructuredBuffer<ParticleInput> ParticleInputBuffer;
StructuredBuffer<EmitterInfo> EmitterInfoBuffer;
RWStructuredBuffer<ParticleOutput> ParticleOutputBuffer;
ConstantBuffer<MVP> mvp;

float4x4 GetBillBoardRotationMatrix(float3 particleAxis, float3 particleWorldPos, float4x4 worldmatrix)
{
    // 축이 없으면 일반 카메라-정면 빌보드
    if (length(particleAxis) < 1e-3)
        return mvp.ViewRotInvMatrix;

    // 카메라→파티클
    float3 viewDir = normalize(mvp.CameraPos.xyz - particleWorldPos);
    float3 forward = -viewDir;

    // 축은 월드 기준으로 고정 (에미터 로컬 회전 영향 제거)
    float3 axisWorld = normalize(particleAxis);

    // 축의 화면 정사영 = 우리가 "가로(right)"로 쓸 방향
    float3 axisProj = axisWorld - dot(axisWorld, forward) * forward;
    float s = length(axisProj); // = sinθ(축‖시선→0, 수직→1)

    // 평행 폴백: 카메라 right 사용
    float3 camRight = normalize(mvp.ViewRotInvMatrix[0].xyz);
    float3 rightDir = (s > 1e-6) ? (axisProj / s) : camRight;

    // 세로는 화면 평면에서 right와 직교
    float3 upDir = normalize(cross(forward, rightDir));

    // 가로만 각도 기반 축소, 세로 유지
    float3 rightScaled = rightDir * s;
    float3 upScaled = upDir;

    return float4x4(
        rightScaled.x, rightScaled.y, rightScaled.z, 0,
        upScaled.x, upScaled.y, upScaled.z, 0,
        forward.x, forward.y, forward.z, 0,
        0, 0, 0, 1
    );
}

[numthreads(64, 1, 1)]
void cs_main(uint3 DTid : SV_DispatchThreadID)
{
    uint idx = DTid.x;

    ParticleInput input = ParticleInputBuffer[idx];
    EmitterInfo emitter = EmitterInfoBuffer[input.EmitterIndex];
    ParticleOutput output;

    float3 acceleration = float3(0, -9.8, 0) * input.Mass;
    float3 gravityOffset = acceleration * input.Age;

    float ratio = saturate(input.Age / emitter.Particlelifetime.x);
    float dragCoef = emitter.DragForce.z;
    float decay = exp(-dragCoef * input.Age);

    float useWorldSpace = emitter.Particlelifetime.y;
    float useWorldScale = clamp(emitter.DragForce.y,0.f,1.f);
    float3 emitterCenter = emitter.WorldMatrix[3].xyz * useWorldSpace;

    float3 r = input.Position.xyz - emitterCenter;
    float3 vortexAxis = emitter.VortexForce.xyz;
    float3 localAxisDir = normalize(vortexAxis);
    float3 worldAxisDir = mul(localAxisDir, (float3x3) emitter.OrientedWorldMatrix);
    float3 axisDir = lerp(localAxisDir, worldAxisDir, useWorldSpace);
    float vortexAtten = emitter.VortexForce.w;
    float vortexStrength = length(vortexAxis);
    float strength = vortexStrength / (1.0 + vortexAtten * length(r));
    float3 vortexVelocity = cross(axisDir, r) * strength;
    float3 vortexDisp = vortexVelocity * input.Age;

    float3 dragPos = (input.Velocity / max(dragCoef, 0.01f)) * (1 - decay);
    float3 totalVel = dragPos + vortexDisp;

    // 색
    float3 col = lerp(emitter.StartColor.rgb, emitter.EndColor.rgb, ratio);
    float opa = lerp(emitter.StartColor.a, emitter.EndColor.a, ratio);
    output.Color = float4(col, opa);

    // === 스케일 하한 보장(축 활성화 시에도 0으로 죽지 않게) ===
    float axisLenInput = length(input.Axis);
    float velLen = length(totalVel);
    float axisfactor = (axisLenInput < 1e-3) ? 1.0f : max(1.0f, velLen); 

    float4 scalefactor = lerp(emitter.StartScale, emitter.EndScale, ratio) * axisfactor ;
    float4x4 scaleMat = CreateScaleMatrix(scalefactor);

    
    scaleMat = mul(scaleMat, lerp(IdentityMatrix, CreateScaleMatrix(float4(ExtractScale(emitter.OrientedWorldMatrix), 1)), useWorldScale));
    
    // 월드 위치 적분
    float4 worldPos = mul(float4(input.Position.xyz, 1.0), emitter.WorldMatrix);
    float4 finalVelocity = mul(float4(totalVel, 0.0f), emitter.OrientedWorldMatrix);
    worldPos.xyz += finalVelocity + gravityOffset * input.Age;

    // VS용 위치(옵션)
    output.Position = mul(worldPos, mvp.ViewMatrix);

    // 메타
    output.Paddings = input.SpriteRotation; // 구조체에 맞게 유지
    output.EmitterIndex = input.EmitterIndex;

    // 행렬 조합
    float4x4 translation = float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        worldPos.x, worldPos.y, worldPos.z, 1
    );

    float4x4 rotation;
    if (length(input.SpriteRotation) < 1e-3)
    {
        rotation = GetBillBoardRotationMatrix(input.Axis, worldPos.xyz, (float4x4) 0);
    }
    else
    {
        rotation = CreateRotationMatrix(input.SpriteRotation);
    }

    output.FinalMatrix = mul(scaleMat, rotation);
    output.FinalMatrix = mul(output.FinalMatrix, translation);
    output.FinalMatrix = mul(output.FinalMatrix, mvp.ViewMatrix);
    output.FinalMatrix = mul(output.FinalMatrix, mvp.ProjMatrix);

    output.FrameInfo = input.Frameinfo;

    ParticleOutputBuffer[idx] = output;
}
