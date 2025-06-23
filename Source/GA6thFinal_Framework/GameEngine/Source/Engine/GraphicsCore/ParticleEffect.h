#pragma once
#include "ParticleHelper.h"

class ParticleEffect
{
    UMPARTICLE_PROPERTY_REF(Vector3, _scale, Scale, Vector3(1, 1, 1));
    UMPARTICLE_PROPERTY_REF(Quaternion, _rotation, Rotation, Quaternion::Identity);
    UMPARTICLE_PROPERTY_REF(Vector3, _position, Position , Vector3(0,0,0));
    UMPARTICLE_PROPERTY(float, _age, Age,0.f);
    UMPARTICLE_PROPERTY(float, _lifetime, Lifetime,5.f);
    UMPARTICLE_PROPERTY(bool, _activeFlag, ActiveFlag, true);




public:
    void Initialize(class ParticleManager* particleManager);
    void Update(float deltaTime);
    ParticleEmitter* AddEmitter(SIZE_T maxParticles = 100000, float emissionRate = 500.f, float emitterLifetime = 5.f,
                    LocationShape locatorShape = LocationShape::SPHERE, Vector3 locationFactor = Vector3(1, 1, 1));
    void RemoveEmitter();
    class ParticleEmitter*              GetEmitter(size_t emitterIndex);
    std::vector<class ParticleEmitter*> GetEmitterList() { return _particleEmitters; }

    void UpdateParticleLifeCycle(float deltaTime);

protected:

    Matrix                             _rotationMatrix;
    Matrix                             _translationMatrix;
    Matrix                             _worldMatrix;
    std::vector<class ParticleEmitter*> _particleEmitters;

};
