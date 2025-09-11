#pragma once
#include "WeaponElement/WeaponElement.h"

/*
*Player의 무기를 관리하는 시스템입니다.
*/
class WeaponSystem : public Component
{
    USING_PROPERTY(WeaponSystem)
    inline static WeaponSystem* static_instance = nullptr;

public:
    inline static constexpr size_t EQUIP_WEAPONS_SIZE = 4;
    inline static WeaponSystem* GetInstance() 
    { 
        if (static_instance)
        {
            if (false == static_instance->gameObject->IsValid())
            {
                static_instance = nullptr;
            }
        }
        return static_instance; 
    }
    REFLECT_PROPERTY(CurrentWeaponSlot)

public:
    WeaponSystem();
    ~WeaponSystem() override;

public:
    GETTER_ONLY(int, CurrentWeaponSlot) { return _currentWeaponSlot; }
    // 현재 사용중인 무기의 슬롯 번호 입니다.
    PROPERTY(CurrentWeaponSlot)

    /// <summary>
    /// 장착된 무기의 원본 Stats을 인덱스로 반환합니다.
    /// </summary>
    /// <param name="index :">가져올 무기 인덱스</param>
    /// <returns></returns>
    WeaponStats& GetWeaponStatsAtIndex(int index) 
    { 
        static WeaponStats null = []() {
            WeaponStats nullSword;
            nullSword.SetName(STR_NULL);
            return nullSword;
        }();
        try
        {
            return _equipWeapons.at(index).Stats;
        }
        catch (const std::exception&)
        {
            UmLogger.Log(LogLevel::LEVEL_ERROR, "out of index!");
        }
        return null;
    }

    /// <summary>
    /// 현재 사용중인 무기의 Stats을 반환합니다.
    /// </summary>
    /// <returns></returns>
    WeaponStats& GetCurrentWeaponStats() { return _equipWeapons[_currentWeaponSlot].Stats; }

    /// <summary>
    /// 무기를 slot에 장착합니다.
    /// 기존에 장착되어있던 무기는 반환되며 제거됩니다.
    /// 잘못된 slot을 접근시 WeaponStats는 0 damege 무기를 반환합니다.
    /// </summary>
    /// <param name="slot :">장착할 슬롯</param>
    WeaponElement EquipWeapon(int slot, const WeaponElement& weapon);

    /// <summary>
    /// 플레이어가 장착중인 무기 항목을 전부 반환합니다.
    /// </summary>
    /// <returns></returns>
    const std::array<WeaponElement, EQUIP_WEAPONS_SIZE>& GetEquipWeapons() { return _equipWeapons; }

    /// <summary>
    /// 현재 사용할 무기를 선택합니다.
    /// 속도, 데미지, 타격 횟수 등이 해당 슬롯에 무기로 설정됩니다.
    /// </summary>
    void SetCurrentWeaponSlot(int slot);

    /// <summary>
    /// 특정 슬롯에 무기 라운드 속도를 반환합니다. (RandomSpeed + Speed)
    /// </summary>
    /// <param name="slot :">반환할 무기 슬롯 </param>
    /// <returns>해당 슬롯 무기의 속도        </returns>
    int GetRoundSpeedToSlot(int slot);

    /// <summary>
    /// 모든 무기에 Random Speed를 계산합니다.
    /// </summary>
    void RollRandomSpeed();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::array<std::string, EQUIP_WEAPONS_SIZE> EquipWeaponsData;
    REFLECT_FIELDS_END(WeaponSystem)

    void Reset() override;
    void Awake() override;

private:

    /*현재 사용중인 무기 슬롯*/
    int _currentWeaponSlot = 0;

    /*장착된 무기들*/
    std::array<WeaponElement, EQUIP_WEAPONS_SIZE> _equipWeapons;

    /*이번 라운드의 Weapon Speed의 순서 (속도 기준 내림차순)*/

    /*에디터용 무기 Imgui 함수*/
    void ImguiEquipWeapons();

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

public:


};
