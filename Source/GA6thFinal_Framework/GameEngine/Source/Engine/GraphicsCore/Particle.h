#pragma once
#include "ParticleHelper.h"

class Particle
{
    UMPARTICLE_PROPERTY_REF(Vector4, _position, Position, Vector4(0, 0, 0, 0));
    UMPARTICLE_PROPERTY_REF(Vector4, _frameinfo, Frameinfo, Vector4(0, 0, 0, 0));
    UMPARTICLE_PROPERTY_REF(Vector3, _axis, Axis, Vector3(0, 0, 0));
    UMPARTICLE_PROPERTY(float, _age, Age,0);
    UMPARTICLE_PROPERTY_REF(Vector3, _velocity, Velocity, Vector3(0, 0, 0));
    UMPARTICLE_PROPERTY(float, _mass, Mass,0.0f);
    UMPARTICLE_PROPERTY(UINT, _emitterIndex, EmitterIndex,0);
    Vector3 padding;
};
