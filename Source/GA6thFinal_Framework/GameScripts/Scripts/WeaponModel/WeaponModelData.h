#pragma once
#include <Stats/Weapon/WeaponStats.h>

class AnimationComponent;
class ParticleComponent;

class WeaponModelData
{
    friend class WeaponModelManager;

public:
    WeaponModelData() : _index(SIZE_MAX) {}
    WeaponModelData(size_t index, WeaponType type, GameObject* gameObject, AnimationComponent* animation,
                    ParticleComponent* particle)
        : _index(index), Type(type), GameObject(gameObject), Animation(animation), Particle(particle)
    {
    }

    const WeaponType    Type       = WeaponType::SWORD;
    GameObject*         GameObject = nullptr;
    AnimationComponent* Animation  = nullptr;
    ParticleComponent*  Particle   = nullptr;

    inline bool IsValid() const { return GameObject != nullptr && Animation != nullptr && Particle != nullptr; }

private:
    size_t _index = SIZE_MAX;
};