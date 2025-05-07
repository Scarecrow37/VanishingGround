#pragma once

#define PROP_DECL(type, varName)                                                                                       \
protected:                                                                                                             \
    type varName;

#define PROP_GET(type, varName, FuncName)                                                                              \
public:                                                                                                                \
    const type& Get##FuncName() const                                                                                  \
    {                                                                                                                  \
        return varName;                                                                                                \
    }

#define PROP_SET(type, varName, FuncName)                                                                              \
public:                                                                                                                \
    void Set##FuncName(const type& value)                                                                              \
    {                                                                                                                  \
        varName = value;                                                                                               \
    }



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

   PROP_DECL(Vector3, _position)
    PROP_GET(Vector3, _position, Position)
    PROP_SET(Vector3, _position, Position)

    PROP_DECL(Vector3, _velocity)
    PROP_GET(Vector3, _velocity, Velocity)
    PROP_SET(Vector3, _velocity, Velocity)

    PROP_DECL(float, _age)
    PROP_GET(float, _age, Age)
    PROP_SET(float, _age, Age)

    PROP_DECL(Vector3, _startVelocity)
    PROP_GET(Vector3, _startVelocity, StartVelocity)
    PROP_SET(Vector3, _startVelocity, StartVelocity)

    PROP_DECL(Vector3, _endVelocity)
    PROP_GET(Vector3, _endVelocity, EndVelocity)
    PROP_SET(Vector3, _endVelocity, EndVelocity)

    PROP_DECL(Vector4, _startColor)
    PROP_GET(Vector4, _startColor, StartColor)
    PROP_SET(Vector4, _startColor, StartColor)

    PROP_DECL(Vector4, _endColor)
    PROP_GET(Vector4, _endColor, EndColor)
    PROP_SET(Vector4, _endColor, EndColor)

    PROP_DECL(Vector2, _startScale)
    PROP_GET(Vector2, _startScale, StartScale)
    PROP_SET(Vector2, _startScale, StartScale)

    PROP_DECL(Vector2, _endScale)
    PROP_GET(Vector2, _endScale, EndScale)
    PROP_SET(Vector2, _endScale, EndScale)

    PROP_DECL(float, _startOpacity)
    PROP_GET(float, _startOpacity, StartOpacity)
    PROP_SET(float, _startOpacity, StartOpacity)

    PROP_DECL(float, _endOpacity)
    PROP_GET(float, _endOpacity, EndOpacity)
    PROP_SET(float, _endOpacity, EndOpacity)

    PROP_DECL(float, _lifetime)
    PROP_GET(float, _lifetime, Lifetime)
    PROP_SET(float, _lifetime, Lifetime)

    PROP_DECL(Matrix, _scaleMatrix)
    PROP_GET(Matrix, _scaleMatrix, ScaleMatrix)
    PROP_SET(Matrix, _scaleMatrix, ScaleMatrix)

    PROP_DECL(Matrix, _rotationMatrix)
    PROP_GET(Matrix, _rotationMatrix, RotationMatrix)
    PROP_SET(Matrix, _rotationMatrix, RotationMatrix)

    PROP_DECL(Matrix, _translationMatrix)
    PROP_GET(Matrix, _translationMatrix, TranslationMatrix)
    PROP_SET(Matrix, _translationMatrix, TranslationMatrix)

    PROP_DECL(Matrix, _worldMatrix)
    PROP_GET(Matrix, _worldMatrix, WorldMatrix)
    PROP_SET(Matrix, _worldMatrix, WorldMatrix)

    PROP_DECL(Matrix, _parentWorldMatrix)
    PROP_GET(Matrix, _parentWorldMatrix, ParentWorldMatrix)
    PROP_SET(Matrix, _parentWorldMatrix, ParentWorldMatrix)

    PROP_DECL(Vector4, _frameinfo)
    PROP_GET(Vector4, _frameinfo, Frameinfo)
    PROP_SET(Vector4, _frameinfo, Frameinfo)

    PROP_DECL(float, _animTimer)
    PROP_GET(float, _animTimer, AnimTimer)
    PROP_SET(float, _animTimer, AnimTimer)

    PROP_DECL(float, _animDuration)
    PROP_GET(float, _animDuration, AnimDuration)
    PROP_SET(float, _animDuration, AnimDuration)

    PROP_DECL(bool, _isLoop)
    PROP_GET(bool, _isLoop, IsLoop)
    PROP_SET(bool, _isLoop, IsLoop)

};
