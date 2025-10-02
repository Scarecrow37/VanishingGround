#pragma once
#include "ParticleHelper.h"

class ParticleEffect
{
public:
    ParticleEffect();
    virtual ~ParticleEffect();
    ParticleEmitter* AddEmitter(SIZE_T maxParticles = 100000, float emissionRate = 500.f, float emitterLifetime = 5.f,
                                LocationShape     locatorShape   = LocationShape::SPHERE,
                                Vector3           locationFactor = Vector3(1, 1, 1),
                                ParticleType      particleType   = ParticleType::SPRITE,
                                std::wstring_view meshspritePath = L"");
    void                                Update(float deltaTime);
    class ParticleEmitter*              GetEmitter(size_t emitterIndex);
    std::vector<class ParticleEmitter*> GetEmitterList() { return _particleEmitters; }
    void                                RemoveEmitter(ParticleEmitter* target);
    void                                UpdateParticleLifeCycle(float deltaTime);
    void                                Play();
    void                                Stop();
    void                                Reset();
    void                                FlushEmitters();
    void                                SetRemoveFlag(bool flag) { _removeFlag = flag; }
    bool                                GetRemoveFlag() const { return _removeFlag; }
    void                                SetPosition(Vector3* position);
    void                                SetRotation(Vector3* rotation);
    void                                SetScale(Vector3* scale);
    void                                SetBoneFollowFlag(bool* flag);
    void                                SetBoneMatrix(const Matrix* matrix);

protected:
    UMPARTICLE_PROPERTY(std::string, _effectName, EffectName, "");
    UMPARTICLE_PROPERTY(float, _age, Age, 0.f);
    UMPARTICLE_PROPERTY(float, _lifetime, Lifetime, 5.f);
    UMPARTICLE_PROPERTY(bool, _activeFlag, ActiveFlag, false);
    UMPARTICLE_PROPERTY(bool, _playFlag, PlayFlag, false);

protected:
    std::vector<class ParticleEmitter*> _particleEmitters;
    Matrix                              _rotationMatrix     = Matrix::Identity;
    Matrix                              _translationMatrix  = Matrix::Identity;
    Matrix                              _scaleMatrix        = Matrix::Identity;
    Matrix                              _worldMatrix        = Matrix::Identity;
    Vector3*                            _position           = nullptr;
    Vector3*                            _rotation           = nullptr;
    Vector3*                            _scale              = nullptr;
    bool                                _isPlaying          = false;
    bool                                _isEnding           = false;
    int                                 _namingIndex        = 0;
    int                                 _emitterNamingIndex = 0;
    bool*                               _followBoneFlag     = nullptr;
    bool                                _removeFlag         = false;
    const Matrix*                       _parentWorldMatrix;
    const Matrix*                       _boneWorldMatrix;
};
