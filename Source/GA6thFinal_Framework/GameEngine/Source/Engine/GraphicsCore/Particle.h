#pragma once
#include "ParticleHelper.h"

class Particle
{
protected:
    Vector4 _color;


public:
    Vector3 GetColor() const { return Vector3{_color.x, _color.y, _color.z}; }
    void    SetColor(const Vector3& color)
    {
        _color.x = color.x;
        _color.y = color.y;
        _color.z = color.z;
    }

    // w: Opacity (float)
    float GetOpacity() const { return _color.w; }
    void  SetOpacity(float opacity) { _color.w = opacity; }
    UMPARTICLE_PROPERTY(UINT, _emitterIndex, EmitterIndex);
    UMPARTICLE_PROPERTY(UINT, _effectIndex, EffectIndex);
    UMPARTICLE_PROPERTY_REF(Vector3, _position, Position);
    UMPARTICLE_PROPERTY_REF(Vector3, _velocity, Velocity);
    UMPARTICLE_PROPERTY(float, _age, Age);
    UMPARTICLE_PROPERTY_REF(Vector3, _startVelocity, StartVelocity);
    UMPARTICLE_PROPERTY_REF(Vector3, _endVelocity, EndVelocity);
    UMPARTICLE_PROPERTY_REF(Vector4, _startColor, StartColor);
    UMPARTICLE_PROPERTY_REF(Vector4, _endColor, EndColor);
    UMPARTICLE_PROPERTY_REF(Vector2, _startScale, StartScale);
    UMPARTICLE_PROPERTY_REF(Vector2, _endScale, EndScale);
    UMPARTICLE_PROPERTY(float, _startOpacity, StartOpacity);
    UMPARTICLE_PROPERTY(float, _endOpacity, EndOpacity);
    UMPARTICLE_PROPERTY(float, _lifetime, Lifetime);
    UMPARTICLE_PROPERTY_REF(Matrix, _scaleMatrix, ScaleMatrix);
    UMPARTICLE_PROPERTY_REF(Matrix, _rotationMatrix, RotationMatrix);
    UMPARTICLE_PROPERTY_REF(Matrix, _translationMatrix, TranslationMatrix);
    UMPARTICLE_PROPERTY_REF(Matrix, _worldMatrix, WorldMatrix);
    UMPARTICLE_PROPERTY_REF(Matrix, _parentWorldMatrix, ParentWorldMatrix);
    UMPARTICLE_PROPERTY_REF(Vector4, _frameinfo, Frameinfo);
    UMPARTICLE_PROPERTY(float, _animTimer, AnimTimer);
    UMPARTICLE_PROPERTY(float, _animDuration, AnimDuration);
    UMPARTICLE_PROPERTY(bool, _isLoop, IsLoop);

};
