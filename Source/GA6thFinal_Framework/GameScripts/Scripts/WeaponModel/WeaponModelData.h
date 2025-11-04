#pragma once
#include <Stats/Weapon/WeaponStats.h>

class AnimationComponent;
class ParticleComponent;

class WeaponModelData
{
    friend class WeaponModelManager;

public:
    WeaponModelData() : _index(SIZE_MAX) {}
    WeaponModelData(size_t index, WeaponType type, std::weak_ptr<GameObject> weakObject, AnimationComponent* animation,
                    ParticleComponent* particle)
        : _index(index), Type(type), GameObject(weakObject), Animation(animation), Particle(particle)
    {
    }

    const WeaponType            Type       = WeaponType::SWORD;
    std::weak_ptr<GameObject>   GameObject = {};
    AnimationComponent*         Animation  = nullptr;
    ParticleComponent*          Particle   = nullptr;

    inline bool IsValid() const
    {
        return _index != SIZE_MAX && Animation != nullptr && Particle != nullptr;
    }
    inline void Active(bool actvie)
    { 
        if (auto gameObject = GameObject.lock())
        {
            gameObject->ActiveSelf = actvie;
        }
    }

private:
    size_t _index = SIZE_MAX;
};