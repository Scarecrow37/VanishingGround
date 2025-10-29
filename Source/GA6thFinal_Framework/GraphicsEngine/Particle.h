#pragma once
#include "ParticleHelper.h"

class Particle
{
public:
    Particle() = default;
    Particle(const Particle& other)
        : _position(other._position), _frameinfo(other._frameinfo), _axis(other._axis), _age(other._age),
          _velocity(other._velocity), _mass(other._mass), _emitterIndex(other._emitterIndex),
          _spriteRotation(other._spriteRotation),
          _initialMatrix(other._initialMatrix)
    {}

    UMPARTICLE_PROPERTY_REF(Vector4, _position, Position, Vector4(0, 0, 0, 0));
    UMPARTICLE_PROPERTY_REF(Vector4, _frameinfo, Frameinfo, Vector4(0, 0, 0, 0));
    UMPARTICLE_PROPERTY_REF(Vector3, _axis, Axis, Vector3(0, 0, 0));
    UMPARTICLE_PROPERTY(float, _age, Age,0);
    UMPARTICLE_PROPERTY_REF(Vector3, _velocity, Velocity, Vector3(0, 0, 0));
    UMPARTICLE_PROPERTY(float, _mass, Mass,0.0f);
    UMPARTICLE_PROPERTY(UINT, _emitterIndex, EmitterIndex,0);
    UMPARTICLE_PROPERTY_REF(Vector3, _spriteRotation, SpriteRotation, Vector3(0, 0, 0));
    UMPARTICLE_PROPERTY_REF(Matrix, _initialMatrix, InitialMatrix, Matrix::Identity);
};
