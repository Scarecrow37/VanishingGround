#pragma once
#include <Stats/Weapon/WeaponStats.h>
#include <Utility/SingletonHelper.h>

class AnimationComponent;
class ParticleComponent;
class WeaponModelManager;

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

class WeaponModelManager : public Component
{
    USING_PROPERTY(WeaponModelManager)

    using AnimationPool = std::vector<class AnimationComponent*>;
    using ParticlePool  = std::vector<class ParticleComponent*>;

public:
    WeaponModelManager();
    ~WeaponModelManager();

    const File::Guid& GetWeaponPrefabGuid(WeaponType type) const;
    Vector3 GetWeaponOffset(WeaponType type) const;

    WeaponModelData RequestAvailableWeapon(WeaponType type);
    bool            ReturnWeaponModel(WeaponModelData data);

private:
    void Awake() override;
    void Start() override;
    void Update() override;
    void OnDestroy() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

    void UpdateOffsetPosition();

private:
    SingletonComponent<WeaponModelManager> _singletonComponent{this};

    std::unordered_map<WeaponType, File::Guid>         _weaponPrefabGuidTable;
    std::unordered_map<WeaponType, AnimationPool>      _weaponAnimationTable;
    std::unordered_map<WeaponType, ParticlePool>       _weaponParticleTable;
    std::unordered_map<WeaponType, std::stack<size_t>> _availableWeaponIndicesTable;

    std::unordered_map<WeaponType, Vector3>            _availableWeaponOffsetsTable;

    REFLECT_FIELDS_BEGIN(Component)
    std::unordered_map<std::string, std::string> WeaponPrefabGuidTable;
    std::unordered_map<std::string, std::array<float, 3>> WeaponPrefabOffsetTable;
    REFLECT_FIELDS_END(WeaponModelManager)

    constexpr static int WEAPON_POOLING_SIZE = 10;
};