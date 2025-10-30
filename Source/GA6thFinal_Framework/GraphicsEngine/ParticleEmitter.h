#pragma once
#include "ParticleHelper.h"
#include "EmitLocator.h"
#include "ParticleRenderModule.h"

class ParticleEmitter
{
    UMPARTICLE_PROPERTY(std::string, _emitterName, EmitterName, "");
    UMPARTICLE_PROPERTY(bool, _activeFlag, ActiveFlag, true);
    bool _endFlag = false;
    bool _particleEndFadeOnceFlag = false;
    bool GetEndFlag() { return _endFlag; }
    void SetEndFlag(bool flag)
    {
        _endFlag                 = flag;
        _particleEndFadeOnceFlag = flag;
    }
    UMPARTICLE_PROPERTY(bool, _removeFlag, RemoveFlag, false);

    UMPARTICLE_PROPERTY(SIZE_T, _maxParticles, MaxParticles, 10000);
    UMPARTICLE_PROPERTY(float, _startDelay, StartDelay, 0.f);
    UMPARTICLE_PROPERTY(float, _emitterAge, EmitterAge, 0.f);
    UMPARTICLE_PROPERTY(float, _emitterLifetime, EmitterLifetime, 5.f);
    UMPARTICLE_PROPERTY(float, _emissionRate, EmissionRate, 5000.f);
    UMPARTICLE_PROPERTY(bool, _spawnBurstFlag, SpawnBurstFlag, false);
    UMPARTICLE_PROPERTY(float, _spawnBurstCount, SpawnBurstCount, 5000);
    UMPARTICLE_PROPERTY(float, _particleLifetime, ParticleLifetime, 1.f);
    float _originParticleLifetime = 0.f;
    UMPARTICLE_PROPERTY(float, _particleMass, ParticleMass, 0.1f);

    UMPARTICLE_PROPERTY(bool, _useLight, UseLight, false);
    UMPARTICLE_PROPERTY(float, _lightIntensity, LightIntensity, 0);
    UMPARTICLE_PROPERTY(float, _lightRange, LightRange, 0);
    UMPARTICLE_PROPERTY_REF(Vector3, _lightColor, LightColor, Vector3(0, 0, 0));

    UMPARTICLE_PROPERTY(bool, _useWorldSpace, UseWorldSpace, true);
    UMPARTICLE_PROPERTY_REF(Matrix, _effectWorldMatrix, EffectWorldMatrix, Matrix::Identity);
    UMPARTICLE_PROPERTY_REF(Vector3, _emitterPosition, EmitterPosition, Vector3(0, 0, 0));
    UMPARTICLE_PROPERTY_REF(Vector3, _particleStartDistributionOffset, ParticleStartDistributionOffset,
                            Vector3(0, 0, 0));
    UMPARTICLE_PROPERTY_REF(Vector3, _particleEndDistributionOffset, ParticleEndDistributionOffset, Vector3(0, 0, 0));
    UMPARTICLE_PROPERTY_REF(Vector3, _particleAxis, ParticleAxis, Vector3(0, 0, 0));
    UMPARTICLE_PROPERTY_REF(Vector3, _particleRotation, ParticleRotation, Vector3(0, 0, 0));
    UMPARTICLE_PROPERTY(bool, _scaleByVelocityFlag, ScaleByVelocityFlag, false);

    UMPARTICLE_PROPERTY_REF(Vector3, _velocity, Velocity, Vector3(1, 1, 1));
    UMPARTICLE_PROPERTY_REF(Vector3, _velocityFactor, VelocityFactor, Vector3(0, 0, 0));

    UMPARTICLE_PROPERTY_REF(Vector3, _startColor, StartColor, Vector3(1, 1, 1));
    UMPARTICLE_PROPERTY(float, _startOpacity, StartOpacity, 0.f);
    UMPARTICLE_PROPERTY_REF(Vector3, _endColor, EndColor, Vector3(1, 1, 1));
    UMPARTICLE_PROPERTY(float, _endOpacity, EndOpacity, 1.f);
    UMPARTICLE_PROPERTY_REF(Vector4, _startScale, StartScale, Vector4(1, 1, 1, 1));
    UMPARTICLE_PROPERTY_REF(Vector4, _endScale, EndScale, Vector4(1, 1, 1, 1));

    UMPARTICLE_PROPERTY_REF(Vector4, _dragPoint, DragPoint, Vector4(0, 0, 0, 0));
    UMPARTICLE_PROPERTY_REF(Vector4, _dragForce, DragForce, Vector4(0, 0, 0, 0));
    UMPARTICLE_PROPERTY_REF(Vector4, _vortexForce, VortexForce, Vector4(0.00001f, 0.f, 0.f, 0));

public:
    ParticleEmitter();
    ~ParticleEmitter();

    ParticleEmitter(const ParticleEmitter&)                = delete;
    ParticleEmitter& operator=(const ParticleEmitter&)     = delete;
    ParticleEmitter(ParticleEmitter&&) noexcept            = default;
    ParticleEmitter& operator=(ParticleEmitter&&) noexcept = default;

    /// 렌더 타입 / 로케이터 타입 (값 타입)
    ParticleType      _particleType = ParticleType::SPRITE;
    LocationShape     _locationType = LocationShape::SPHERE;
    VelocityScaleType _velocityType = VelocityScaleType::LINEAR;

    std::unique_ptr<ParticleRenderModule> _particleRenderModule;
    std::unique_ptr<EmitLocator>          _emitLocator;
    GraphicsPointer<class ILight>         _particlePointLight;

    void SetLocatorFactor(const Vector3& factor);
    void SetVelocityType(VelocityScaleType velType);

public:
    void Initialize(SIZE_T maxParticles = 10000, float emissionRate = 500.f, float emitterLifetime = 5.f,
                    LocationShape locatorShape = LocationShape::SPHERE, Vector3 locationFactor = Vector3(1, 1, 1),
                    ParticleType particleType = ParticleType::SPRITE, const std::wstring& meshspritePath = L"");
    void Update(float deltaTime);
    void UpdateParticleLifeCycle(float deltaTime);
    void FlushTextureResource();
    void Reset();

    inline std::vector<class Particle>& GetParticlePool() { return _particlePool; }
    UINT                                GetActiveParticleCount() const { return (UINT)_activeParticleCount; }
    Matrix                              GetWorldMatrix() const { return _worldMatrix; }
    const Quaternion&                   GetEmitterRotationQ() const { return _emitterRotationQ; }
    void                                SetEmitterRotationQ(const Quaternion& value);
    const Vector3&                      GetEmitterRotationE() const { return _emitterRotationE; }
    void                                SetEmitterRotationE(const Vector3& value);

    void InitializeLight(std::string_view scenenName);
    void InitializeEditorLight();

protected:
    void InitializeLocator(LocationShape locatorShape, Vector3 factor);
    void AwakeParticle(UINT index);
    void ScaleVelocity(Vector3 pos);
    void ScaleVelFromPoint(Vector3 pos);
    void ScaleVelInCone(Vector3 pos);

    std::vector<class Particle> _particlePool;
    SIZE_T                      _activeParticleCount = 0;
    Matrix                      _translationMatrix   = Matrix::Identity;
    Matrix                      _rotationMatrix      = Matrix::Identity;
    Matrix                      _worldMatrix         = Matrix::Identity;
    Quaternion                  _emitterRotationQ    = Quaternion::Identity;
    Vector3                     _emitterRotationE    = Vector3(0, 0, 0);
    Vector3                     _finalPos            = Vector3(0, 0, 0);

    bool  _delayFlag         = false;
    float _delayTimer        = 0.f;
    bool  _isSpawnBursted    = false;
    float _emissionThreshold = 0;

    float   _lightCurrentIntensity   = 0;
    float   _lightCurrentRange       = 0;
    Vector3 _lightAttenuation        = Vector3(0, 0, 0);
    float   _endLightIntensity       = 0;

    std::function<Vector3(void)> _velocityScalingFunciton = nullptr; // 기존 이름 유지
};
