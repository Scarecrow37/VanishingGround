#include "pch.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "ParticleEffect.h"

 ParticleEffect::~ParticleEffect() 
 {
     // 1. ParticleEmitter 객체들 정리
     for (auto emitter : _particleEmitters)
     {
         delete emitter;
     }
     _particleEmitters.clear();
 }

void ParticleEffect::Initialize(class ParticleManager* particleManager) 
{
    //float random = rand() % 10;
    //_scale = Vector3(random, random, random);
}

void ParticleEffect::Update(float deltaTime) 
{

   _age += deltaTime;
    if (_age >= _lifetime)
    {
        _activeFlag = false;
        return;
    }

    _rotationMatrix = Matrix::CreateFromQuaternion(_rotation);
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
                                Vector3 locationFactor /*= Vector3(1, 1, 1)*/)
{
    auto newEmitter = new ParticleEmitter();
    newEmitter->Initialize(maxParticles, emissionRate, emitterLifetime, locatorShape, locationFactor);

    _particleEmitters.push_back(newEmitter);
    return newEmitter;
}

void ParticleEffect::RemoveEmitter() {}

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
