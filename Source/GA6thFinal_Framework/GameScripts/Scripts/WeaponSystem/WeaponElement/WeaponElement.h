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
    bool        IsAction() const { return _action != nullptr; }
    TurnAction& GetAction() { return *_action; }

protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    std::string WeaponStatsData = STR_NULL;
    std::string ActionDatas     = STR_NULL;
    std::string ActionName      = STR_NULL;
    REFLECT_FIELDS_END(WeaponElement)

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

private:
    std::unique_ptr<TurnAction> _action;
    void DeepCopyAction(const TurnAction& rhs);
public:
    WeaponElement& CopyElement(const WeaponElement& rhs) 
    {
        if (this == &rhs)
        {
            return *this;
        }
        Stats = rhs.Stats;
        if (rhs._action)
        {
            DeepCopyAction(*rhs._action);
        }
        else
        {
            _action.reset();
        }
        return *this;     
    }
    WeaponElement(const WeaponElement& rhs) { CopyElement(rhs); }
    WeaponElement& operator=(const WeaponElement& rhs) { return CopyElement(rhs); }

private:
    bool _showActionEditor = false;
    bool _showTableActionEditor = false;

public:
    // IDropItem을(를) 통해 상속됨
    DropItemInfo GetItemInfo() const override;
    ArtifactDropType GetCategoryType() const;
};