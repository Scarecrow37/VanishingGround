#pragma once
#include "ParticleHelper.h"

class ParticleEffect
{
    using EffectCallback = std::function<void(void)>;

public:
    ParticleEffect();
    virtual ~ParticleEffect();

    // 문자열 수명 보존을 위해 view 대신 값 복사 사용 권장
    class ParticleEmitter* AddEmitter(SIZE_T maxParticles = 100000, float emissionRate = 500.f, float emitterLifetime = 5.f,
                                LocationShape locatorShape   = LocationShape::SPHERE,
                                Vector3       locationFactor = Vector3(1, 1, 1),
                                      ParticleType        particleType   = ParticleType::SPRITE,
                                      const std::wstring& meshspritePath = L"");

    void                                                 Update(float deltaTime);
    class ParticleEmitter*                               GetEmitter(size_t emitterIndex);
    const std::vector<std::unique_ptr<ParticleEmitter>>& GetEmitterList() const;
    void                                                 RemoveEmitter(ParticleEmitter* target);
    void                                                 UpdateParticleLifeCycle(float deltaTime);
    void                                                 Play();
    void                                                 Play(EffectCallback callback);
    void                                                 Stop();
    void                                                 Reset();
    void                                                 FlushEmitters();
    void                                                 SetRemoveFlag(bool flag);
    bool                                                 GetRemoveFlag() const;
    void                                                 SetPosition(Vector3* position);
    void                                                 SetRotation(Vector3* rotation);
    void                                                 SetScale(Vector3* scale);
    void                                                 SetBoneFollowFlag(bool* flag);
    void                                                 SetBoneMatrix(const Matrix* matrix);
    void                                                 SetParentMatrix(const Matrix* matrix);

    ParticleEffect(const ParticleEffect&)                = delete;
    ParticleEffect& operator=(const ParticleEffect&)     = delete;
    ParticleEffect(ParticleEffect&&) noexcept            = default;
    ParticleEffect& operator=(ParticleEffect&&) noexcept = default;

protected:
    UMPARTICLE_PROPERTY(std::string, _effectName, EffectName, "");
    UMPARTICLE_PROPERTY(float, _age, Age, 0.f);
    UMPARTICLE_PROPERTY(float, _lifetime, Lifetime, 5.f);
    UMPARTICLE_PROPERTY(bool, _activeFlag, ActiveFlag, false);
    UMPARTICLE_PROPERTY(bool, _playFlag, PlayFlag, false);

protected:
    // unique_ptr로 소유
    std::vector<std::unique_ptr<class ParticleEmitter>> _particleEmitters;

    Matrix _rotationMatrix    = Matrix::Identity;
    Matrix _translationMatrix = Matrix::Identity;
    Matrix _scaleMatrix       = Matrix::Identity;
    Matrix _worldMatrix       = Matrix::Identity;

    Vector3* _position = nullptr;
    Vector3* _rotation = nullptr;
    Vector3* _scale    = nullptr;

    bool _isPlaying          = false;
    bool _isEnding           = false;
    int  _namingIndex        = 0;
    int  _emitterNamingIndex = 0;

    bool*         _followBoneFlag    = nullptr;
    bool          _removeFlag        = false;
    const Matrix* _parentWorldMatrix = nullptr;
    const Matrix* _boneWorldMatrix   = nullptr;

    EffectCallback _endCallback = nullptr;
};
