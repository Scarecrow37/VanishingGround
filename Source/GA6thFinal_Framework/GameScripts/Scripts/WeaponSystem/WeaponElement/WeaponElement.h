#pragma once
#include <ItemDropSystem/Interface/IDropItem.h>
#include <TurnSystem/TurnAction/TurnAction.h>
#include <Stats/Weapon/WeaponStats.h>

class WeaponElement : public ReflectSerializer, public IDropItem
{
    friend class WeaponSystem;
    friend class WeaponTableComponent;
    USING_PROPERTY(WeaponElement)
public:
    WeaponElement()           = default;
    ~WeaponElement() override = default;
    REFLECT_PROPERTY()

public:
    WeaponStats Stats;
    const std::vector<std::unique_ptr<TurnAction>>& GetActions() { return _actions; }

protected:
    using ActionNameDataPair = std::pair<std::string, std::string>;
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    std::string WeaponStatsData = STR_NULL;
    std::vector<ActionNameDataPair> Actions;
    REFLECT_FIELDS_END(WeaponElement)

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

private:
    std::vector<std::unique_ptr<TurnAction>> _actions;
    void DeepCopyAction(const std::vector<std::unique_ptr<TurnAction>>& rhs);
public:
    WeaponElement& CopyElement(const WeaponElement& rhs) 
    {
        if (this == &rhs)
        {
            return *this;
        }
        Stats = rhs.Stats;
        DeepCopyAction(rhs._actions);
        return *this;     
    }
    WeaponElement(const WeaponElement& rhs) { CopyElement(rhs); }
    WeaponElement& operator=(const WeaponElement& rhs) { return CopyElement(rhs); }

public:
    // IDropItem을(를) 통해 상속됨
    DropItemInfo GetItemInfo() const override;
    ArtifactDropType GetCategoryType() const;
};