#pragma once
#include "Engine/GraphicsCore/ParticleEffect.h"
#include "Engine/GraphicsCore/ParticleEmitter.h"
#include "UmFramework.h"
class ParticleComponent : public Component
{
    USING_PROPERTY(ParticleComponent)
public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _filepath.string(); }
    PROPERTY(FilePath)

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
    REFLECT_FIELDS_END(ParticleComponent)

    ParticleEffect* _effect;
    void            Update() override;
    void            DeserializedReflectEvent() override;

private:
    float age       = 0.f;
    float lifetime  = 0.f;
    bool isplaying = false;
};
