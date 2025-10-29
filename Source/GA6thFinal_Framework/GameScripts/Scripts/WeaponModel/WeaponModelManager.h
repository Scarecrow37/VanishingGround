#pragma once
#include "Utility/SingletonHelper.h"
#include "WeaponModelData.h"

class AnimationComponent;
class ParticleComponent;
class WeaponModelManager;

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

    const std::string* GetRandomWeaPonAnimationKeyToNormalAttack(WeaponType type);
    const std::string* GetRandomWeaPonAnimationKeyToSpecialAttack(WeaponType type);


private:
    void Awake() override;
    void Start() override;
    void Update() override;
    void OnDestroy() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

    void UpdateOffsetPosition();

    void RegisterWeaponAnimation(WeaponType type, AnimationComponent* component);
    void RegisterWeaponParticle(WeaponType type, ParticleComponent* component);

    void InitializeAnimationList();

private:
    SingletonComponent<WeaponModelManager> _singletonComponent{this};

    std::unordered_map<WeaponType, File::Guid>                  _weaponPrefabGuidTable;
    std::unordered_map<WeaponType, AnimationPool>               _weaponAnimationTable;
    std::unordered_map<WeaponType, ParticlePool>                _weaponParticleTable;
    std::unordered_map<WeaponType, std::stack<size_t>>          _availableWeaponIndicesTable;

    std::unordered_map<WeaponType, Vector3>                     _availableWeaponOffsetsTable;

    std::unordered_map<WeaponType, std::vector<std::string>>    _weaponAnimationNormalNameList;
    std::unordered_map<WeaponType, std::vector<std::string>>    _weaponAnimationSpecialNameList;

    REFLECT_FIELDS_BEGIN(Component)
    std::unordered_map<std::string, std::string> WeaponPrefabGuidTable;
    std::unordered_map<std::string, std::array<float, 3>> WeaponPrefabOffsetTable;
    REFLECT_FIELDS_END(WeaponModelManager)

    constexpr static int WEAPON_POOLING_SIZE = 10;
};