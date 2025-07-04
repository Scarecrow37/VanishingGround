#pragma once
#include "UmFramework.h"
#include <Stats/Weapon/WeaponStats.h>

class WeaponTableComponent : public Component
{
    USING_PROPERTY(WeaponTableComponent)
    inline static WeaponTableComponent* static_instance = nullptr;

public:
    static WeaponTableComponent* GetInstance() { return static_instance; }
public:
    WeaponTableComponent();
    virtual ~WeaponTableComponent();

public:
    REFLECT_PROPERTY()
protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::vector<std::string> _tableDatas;
    REFLECT_FIELDS_END(WeaponTableComponent)

public:
    bool RenameWeapon(WeaponStats& weapon, std::string_view newName);
    bool InsertWeapon(WeaponStats& weapon);
    bool EraseWeapon(WeaponStats& weapon);

private:
    std::map<std::string, WeaponStats> _weaponTable;

private:
    struct ImguiEvent
    {
        std::string DeleteTableBuffer = STR_NULL;
        bool        OpenDeletePopup   = false;

        WeaponStats* SelectWeapon = nullptr;
        std::string  RenameBuffer;
        bool         OpenRenamePopup = false;
    } 
    _imguiEvent;

protected:

    virtual void Awake() override;

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    virtual void ImGuiDrawPropertysEvent() override;

    /// <summary>
    /// <para> 직렬화 직전 자동으로 호출되는 이벤트 함수입니다. </para>
    /// <para> 직접 override 해서 사용합니다.                 </para>
    /// </summary>
    virtual void SerializedReflectEvent() override;

    /// <summary>
    /// <para> 역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.  </para>
    /// <para> 직접 override 해서 사용합니다.                     </para>
    /// </summary>
    virtual void DeserializedReflectEvent() override;


};
