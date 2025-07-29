#include "pch.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "ParticleEffect.h"

 ParticleEffect::~ParticleEffect() 
 {
     // 1. ParticleEmitter 객체들 정리
     for (auto emitter : _particleEmitters)
     {
         if (emitter)
            delete emitter;
     }
     _particleEmitters.clear();
 }



 ParticleEffect::ParticleEffect(const ParticleEffect& other) 
         : _rotation(other._rotation), _position(other._position), _age(0), _lifetime(other._lifetime),
       _activeFlag(other._activeFlag), _effectName(other._effectName), _playFlag(other._playFlag),
       namingIndex(other.namingIndex), emitterNamingIndex(other.emitterNamingIndex)
 {
     _particleEmitters.reserve(other._particleEmitters.size());
     for (auto* srcEmitter : other._particleEmitters)
     {
         // Assumes ParticleEmitter has a proper copy constructor
         ParticleEmitter* cloned = new ParticleEmitter(*srcEmitter);
         cloned->Initialize(srcEmitter->GetMaxParticles(), srcEmitter->GetEmissionRate(),
                            srcEmitter->GetEmitterLifetime(), srcEmitter->_locationType,
                            srcEmitter->_emitLocator->GetFactor(), srcEmitter->_particleType,
                            srcEmitter->_particleRenderModule->GetModelAndTexturePath());
         _particleEmitters.push_back(cloned);
     }
 }

 void ParticleEffect::Initialize(class ParticleManager* particleManager) {}

void ParticleEffect::Update(float deltaTime)
{
    _age += deltaTime;
    if (_age >= _lifetime)
    {
        _activeFlag = false;
        _playFlag   = false;
        if (true == _isPlaying)
        {
            _isPlaying = false;
            _age       = 0;
        }
        return;
    }
    {

        if (nullptr != _position)
            _translationMatrix = Matrix::CreateTranslation(*_position);
        else
            _translationMatrix = Matrix::Identity;

        if (nullptr != _rotation)
            _rotationMatrix = Matrix::CreateFromQuaternion(
                Quaternion::CreateFromYawPitchRoll(*_rotation));
        else
            _rotationMatrix = Matrix::Identity;

        if (nullptr != _scale)
            _scaleMatrix = Matrix::CreateScale(*_scale);
        else
            _scaleMatrix = Matrix::Identity;


    }

    if (false == _followBoneFlag)
    {

        if (nullptr != _parentWorldMatrix)
            _worldMatrix = _scaleMatrix * _rotationMatrix * _translationMatrix * *_parentWorldMatrix;
        else
            _worldMatrix = _scaleMatrix * _rotationMatrix * _translationMatrix;
    }
    else
    {
        _worldMatrix = _scaleMatrix * _rotationMatrix * _translationMatrix *(*_boneWorldMatrix) * *_parentWorldMatrix;
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

ParticleEmitter* ParticleEffect::AddEmitter(SIZE_T maxParticles /*= 100000*/, float emissionRate /*= 500.f*/,
                                float emitterLifetime /*= 5.f*/, LocationShape locatorShape /*= LocationShape::SPHERE*/,
                                            Vector3       locationFactor /*= Vector3(1, 1, 1)*/,
                                            ParticleType  particleType /*= ParticleType::SPRITE*/,
                                            std::wstring_view meshspritePath /*= L""*/)
{
    auto newEmitter = new ParticleEmitter();
    newEmitter->Initialize(maxParticles, emissionRate, emitterLifetime, locatorShape, locationFactor,particleType,meshspritePath);
    std::string name = "Emitter " + std::to_string(namingIndex) + "-" + std::to_string(emitterNamingIndex++);
    newEmitter->SetEmitterName(name);
    _particleEmitters.push_back(newEmitter);
    return newEmitter;
}

void ParticleEffect::RemoveEmitter(ParticleEmitter* target) 
{
    target->SetRemoveFlag(true);
   
}

class ParticleEmitter* ParticleEffect::GetEmitter(size_t emitterIndex) 
{
    return _particleEmitters[emitterIndex];
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
        _isEnding   = false;
        _age        = 0;
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

        _isEnding = true;
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
