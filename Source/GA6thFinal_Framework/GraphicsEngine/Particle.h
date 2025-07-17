#pragma once
#include "ParticleHelper.h"

class Particle
{
    UMPARTICLE_PROPERTY_REF(Vector4, _position, Position,Vector4(0,0,0,0));
    UMPARTICLE_PROPERTY_REF(Vector4, _frameinfo, Frameinfo, Vector4(0, 0, 0, 0));
    UMPARTICLE_PROPERTY_REF(Vector4, _startScale, StartScale , Vector4(0, 0, 0, 0));
    UMPARTICLE_PROPERTY_REF(Vector4, _endScale, EndScale, Vector4(0, 0, 0, 0));

    UMPARTICLE_PROPERTY_REF(Vector3, _color, Color,Vector3(0,0,0));
    UMPARTICLE_PROPERTY(float, _opacity, Opacity,0);
    UMPARTICLE_PROPERTY_REF(Vector3, _startColor, StartColor, Vector3(0, 0, 0));
    UMPARTICLE_PROPERTY(float, _startOpacity, StartOpacity,0);
    UMPARTICLE_PROPERTY_REF(Vector3, _endColor, EndColor, Vector3(0, 0, 0));
    UMPARTICLE_PROPERTY(float, _endOpacity, EndOpacity,0);

    UMPARTICLE_PROPERTY_REF(Vector3, _axis, Axis, Vector3(0, 0, 0));
    UMPARTICLE_PROPERTY(float, _age, Age,0);

    UMPARTICLE_PROPERTY_REF(Vector3, _velocity, Velocity, Vector3(0, 0, 0));
    UMPARTICLE_PROPERTY(UINT, _emitterIndex, EmitterIndex,0);

    UMPARTICLE_PROPERTY_REF(Vector4, _scale, Scale,Vector4(1,1,1,1));
    UMPARTICLE_PROPERTY(float, _lifetime, Lifetime,1.f);
    UMPARTICLE_PROPERTY(float, _mass, Mass,0.1f);
    Vector2 padding;
};
struct ParticleInput
{
public:
    Vector4 frameinfo; // duration, elapsedTime, currentIndex, isLoop
    Vector4 position;
    Vector4 startScale;
    Vector4 endScale;
    Vector3 color;
    float   opacity;
    Vector3 startColor;
    float   startopacity;
    Vector3 endColor;
    float   endopacity;
    Vector3 axis;
    float   age;
    Vector3 velocity;
    int     emitterIndex;
    Vector4 scale;
    float   lifetime;
    float   mass;
    Vector2 paddings;
};