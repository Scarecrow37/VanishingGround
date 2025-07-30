#pragma once
#include <WeaponSystem/WeaponElement/WeaponElement.h>

class WeaponTableComponent : public Component
{
    USING_PROPERTY(WeaponTableComponent)
    inline static WeaponTableComponent* static_instance = nullptr;

public:
    inline static constexpr char TAG[] = "WeaponTable";
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
    /// <summary>
    /// 이름으로 무기를 찾아 Stats를 반환합니다. 존재하지 않으면 nullptr을 반환합니다.
    /// </summary>
    /// <param name="name :">찾을 무기 이름</param>
    /// <returns>찾은 무기 Stats 정보</returns>
    const WeaponElement* GetWeaponToName(std::string_view name);

    /// <summary>
    /// 이름을 key로 사용하는 WeaponTable을 반환합니다.
    /// </summary>
    /// <returns></returns>
    const std::map<std::string, WeaponElement>& GetWeaponTable() { return _weaponTable; }

    std::string SaveWeaponTable();
    bool        LoadWeaponTable(std::string_view data);

private:
    bool RenameWeapon(WeaponElement& weapon, const std::string& newName);
    bool InsertWeapon(WeaponElement& weapon);
    bool EraseWeapon(WeaponElement& weapon);

private:
    std::map<std::string, WeaponElement> _weaponTable;

private:
    struct ImguiEvent
    {
        bool ShowTableEditor = false;

        std::string           DeleteTableBuffer = STR_NULL;
        bool                  OpenDeletePopup   = false;
        WeaponElement*        SelectWeapon      = nullptr;
        std::function<void()> RenameFunc;
    } 
    _imguiEvent;

protected:

    virtual void Reset() override;

    virtual void Awake() override;

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    virtual void ImGuiDrawPropertysEvent() override;

    void ImGuiTableEditor();

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
