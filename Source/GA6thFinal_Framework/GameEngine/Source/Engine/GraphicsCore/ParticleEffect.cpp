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
         : _rotation(Quaternion::Identity), _position(0,0,0), _age(0), _lifetime(other._lifetime),
       _activeFlag(other._activeFlag), _effectName(other._effectName), _playFlag(other._playFlag),
       namingIndex(other.namingIndex), emitterNamingIndex(other.emitterNamingIndex)
 {
     _particleEmitters.reserve(other._particleEmitters.size());
     for (auto* srcEmitter : other._particleEmitters)
     {
         if(nullptr==srcEmitter) continue;
         // Assumes ParticleEmitter has a proper copy constructor
         ParticleEmitter* cloned = new ParticleEmitter(*srcEmitter);
         cloned->Initialize(srcEmitter->GetMaxParticles(),srcEmitter->GetEmissionRate(),srcEmitter->GetEmitterLifetime(),
             srcEmitter->_locationType,srcEmitter->_emitLocator->GetFactor(), srcEmitter->_particleType);
         _particleEmitters.push_back(cloned);
     }
 }

 void ParticleEffect::Initialize(class ParticleManager* particleManager)
 {

}

void ParticleEffect::Update(float deltaTime)
{

    _age += deltaTime;
    if (_age >= _lifetime)
    {
        _activeFlag = false;
        _playFlag   = false;
        return;
    }

    _rotationMatrix    = Matrix::CreateFromQuaternion(_rotation);
    _translationMatrix = Matrix::CreateTranslation(_position);

    _worldMatrix = _rotationMatrix * _translationMatrix;

    for (auto emitter : _particleEmitters)
    {
        emitter->SetEffectWorldMatrix(_worldMatrix);
        emitter->Update(deltaTime);
    }


}

ParticleEmitter* ParticleEffect::AddEmitter(SIZE_T maxParticles /*= 100000*/, float emissionRate /*= 500.f*/,
                                float emitterLifetime /*= 5.f*/, LocationShape locatorShape /*= LocationShape::SPHERE*/,
                                            Vector3       locationFactor /*= Vector3(1, 1, 1)*/,
                                            ParticleType  particleType /*= ParticleType::SPRITE*/)
{
    auto newEmitter = new ParticleEmitter();
    newEmitter->Initialize(maxParticles, emissionRate, emitterLifetime, locatorShape, locationFactor,particleType);
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
    _playFlag = true;
}


void ParticleEffect::Reset() {

}

void ParticleEffect::FlushEmitters() 
{
    // erase removed emitter
    //auto removeEmitterEnd = std::remove_if(_particleEmitters.begin(), _particleEmitters.end(),
    //                                       [](ParticleEmitter* emitter) { return emitter->GetRemoveFlag(); });
    //for (auto it = removeEmitterEnd; it != _particleEmitters.end(); ++it)
    //{
    //    *it = nullptr;
    //}

    //_particleEmitters.erase(removeEmitterEnd, _particleEmitters.end());




    std::erase_if(_particleEmitters, [](ParticleEmitter* emitter) { return emitter->GetRemoveFlag(); });
}
