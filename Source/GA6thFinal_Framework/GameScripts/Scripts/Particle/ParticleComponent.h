#pragma once
#include "Engine/GraphicsCore/ParticleEffect.h"
#include "Engine/GraphicsCore/ParticleEmitter.h"
#include "UmFramework.h"
class ParticleComponent : public Component
{
    USING_PROPERTY(ParticleComponent)
public:
    REFLECT_PROPERTY(FilePath, Position, Rotation, Scale)
    GETTER_ONLY(std::string, FilePath) { return _filepath.string(); }
    PROPERTY(FilePath)

    GETTER(const Vector3&, Position) { return _positionVector; }
    SETTER(const Vector3&, Position)
    {
        _positionVector = value;
        std::memcpy(&ReflectFields->PositionArray[0], &_positionVector.x, sizeof(ReflectFields->PositionArray));
    }
    PROPERTY(Position)

    GETTER(const Vector3&, Rotation) { return _rotationVector; }
    SETTER(const Vector3&, Rotation) 
    { 
        _rotationVector = value;
        std::memcpy(&ReflectFields->RotationArray[0], &_rotationVector.x, sizeof(ReflectFields->RotationArray));
    }
    PROPERTY(Rotation)

    GETTER(const Vector3&, Scale) { return _scaleVector; }
    SETTER(const Vector3&, Scale)
    {
        _scaleVector = value;
        std::memcpy(&ReflectFields->ScaleArray[0], &_scaleVector.x, sizeof(ReflectFields->ScaleArray));
    }
    PROPERTY(Scale)


    GETTER_ONLY(const ParticleEffect*, Effect) { return _effect; }
    PROPERTY(Effect)


    void PlayEffect();

public:
    ParticleComponent();
    virtual ~ParticleComponent();

    File::GuidRef _guidRef;
    File::Path    _filepath;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string Guid;
    std::array<float, 3>        PositionArray;
    std::array<float, 3>        RotationArray;
    std::array<float, 3>        ScaleArray;
    REFLECT_FIELDS_END(ParticleComponent)

    ParticleEffect* _effect;
    void            Update() override;
    void            DeserializedReflectEvent() override;
    void            ImGuiDrawPropertysEvent() override;

private:
    bool  _isPlaying = false;
    float age        = 0.f;
    float lifetime   = 0.f;
    bool  isplaying  = false;
    void  LoadParticle();

    Vector3 _positionVector{0.f, .0f, 0.f};
    Vector3 _rotationVector{0.f, 0.f, 0.f};
    Vector3 _scaleVector{1.f, 1.f, 1.f};

};
