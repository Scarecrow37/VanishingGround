#include "pch.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "ParticleEffect.h"

 ParticleEffect::ParticleEffect() = default;
ParticleEffect::~ParticleEffect()
{
    for (auto emitter : _particleEmitters)
    {
        if (emitter)
            delete emitter;
    }
    _particleEmitters.clear();
}
ParticleEmitter* ParticleEffect::AddEmitter(SIZE_T maxParticles, float emissionRate, float emitterLifetime,
                                            LocationShape locatorShape, Vector3 locationFactor,
                                            ParticleType particleType, std::wstring_view meshspritePath)
{
    auto newEmitter = new ParticleEmitter();
    newEmitter->Initialize(maxParticles, emissionRate, emitterLifetime, locatorShape, locationFactor, particleType,
                           meshspritePath);
    std::string name = "Emitter " + std::to_string(_namingIndex) + "-" + std::to_string(_emitterNamingIndex++);
    newEmitter->SetEmitterName(name);
    _particleEmitters.push_back(newEmitter);
    return newEmitter;
}
void ParticleEffect::Update(float deltaTime)
{
    _age += deltaTime;
    if (_age >= _lifetime)
    {
        _activeFlag = false;
        for (auto emitter : _particleEmitters)
        {
            emitter->SetActiveFlag(false);
        }
        _playFlag = false;
        if (true == _isPlaying)
        {
            _isPlaying = false;
            _age       = 0;
        }
        return;
    }
    {
        _translationMatrix = nullptr != _position ? Matrix::CreateTranslation(*_position) : Matrix::Identity;
        _rotationMatrix    = nullptr != _rotation
                                 ? Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(*_rotation))
                                 : Matrix::Identity;
        _scaleMatrix       = nullptr != _scale ? Matrix::CreateScale(*_scale) : Matrix::Identity;
    }
    if (nullptr != _parentWorldMatrix)
    {
        if (nullptr != _followBoneFlag && false == *_followBoneFlag)
        {
            _worldMatrix = _scaleMatrix * _rotationMatrix * _translationMatrix;
            if (nullptr != _parentWorldMatrix)
            {
                _worldMatrix *= *_parentWorldMatrix;
            }
        }
        else if (nullptr != _boneWorldMatrix)
        {
            _worldMatrix =
                _scaleMatrix * _rotationMatrix * _translationMatrix * (*_boneWorldMatrix) * *_parentWorldMatrix;
        }
    }

    for (auto emitter : _particleEmitters)
    {
        emitter->SetEffectWorldMatrix(_worldMatrix);
        emitter->Update(deltaTime);
    }
    {
        for (auto emitter : _particleEmitters)
        {
            if (true == emitter->GetActiveFlag())
                return;
        }
        _activeFlag = false;
        _playFlag   = false;
        _isEnding   = false;
        if (true == _isPlaying)
        {
            _isPlaying = false;
            _age       = 0;
        }
    }
}
class ParticleEmitter* ParticleEffect::GetEmitter(size_t emitterIndex)
{
    return _particleEmitters[emitterIndex];
}
void ParticleEffect::RemoveEmitter(ParticleEmitter* target)
{
    target->SetRemoveFlag(true);
}
void ParticleEffect::UpdateParticleLifeCycle(float deltaTime)
{
    for (auto emitter : _particleEmitters)
    {
        if (true == emitter->GetActiveFlag())
        {
            emitter->UpdateParticleLifeCycle(deltaTime);
        }
    }
}
void ParticleEffect::Play()
{
    if (false == _isPlaying)
    {
        _playFlag   = true;
        _isPlaying  = true;
        _activeFlag = true;

        _isEnding = false;
        _age      = 0;
        for (auto& emitter : _particleEmitters)
        {
            emitter->Reset();
            emitter->SetActiveFlag(true);
        }
    }
}
void ParticleEffect::Stop()
{
    if (false == _isEnding)
    {
        _isEnding  = true;
        _isPlaying = false;
        for (auto& emitter : _particleEmitters)
        {
            emitter->SetEndFlag(true);
        }
    }
}
void ParticleEffect::Reset()
{
    _age = 0;
    for (auto emitter : _particleEmitters)
    {
        emitter->Reset();
    }
    _activeFlag = true;
}
void ParticleEffect::FlushEmitters()
{
    for (auto it = _particleEmitters.begin(); it != _particleEmitters.end();)
    {
        if ((*it)->GetRemoveFlag())
        {
            delete *it; // 메모리 해제
            it = _particleEmitters.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
void ParticleEffect::SetPosition(Vector3* position)
{
    _position = position;
}
void ParticleEffect::SetRotation(Vector3* rotation)
{
    _rotation = rotation;
}
void ParticleEffect::SetScale(Vector3* scale)
{
    _scale = scale;
}
void ParticleEffect::SetBoneFollowFlag(bool* flag) 
{
    _followBoneFlag = flag;
}
void ParticleEffect::SetBoneMatrix(const Matrix* matrix) 
{
    _boneWorldMatrix = matrix;
}
