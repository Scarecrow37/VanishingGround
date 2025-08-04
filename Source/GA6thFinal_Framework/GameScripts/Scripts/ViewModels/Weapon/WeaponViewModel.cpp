#include "pchScripts.h"
#include "WeaponViewModel.h"

#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "WeaponSystem/WeaponSystem.h"

struct GetWeaponIcon
{
    File::GuidRef operator()(const int weaponId) const
    {
        File::GuidRef portraitGuid;
        switch (weaponId)
        {
        case 1: // 녹슨자의 검
            portraitGuid = File::GuidRef("a22f8532-a669-4e92-869f-93322495e9d6");
            break;
        case 2: // 돌격 대장의 망치
            portraitGuid = File::GuidRef("87beed06-2b3b-4846-bf85-111862916000");
            break;
        case 3: // 돌파자의 장검
            portraitGuid = File::GuidRef("a9516b97-9ff7-4fef-81e9-8736ed5ad3a7");
            break;
        case 4: // 제물의 단검
            portraitGuid = File::GuidRef("2d779a5a-e55a-4334-890f-135ec2167155");
            break;
        default:
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Unknown weapon ID: " + std::to_string(weaponId));
            portraitGuid = File::NULL_GUID; // Default to SWORD
            break;
        }
        return portraitGuid;
    }
};

struct GetWeaponBackground
{
    File::GuidRef operator()(const WeaponType type) const
    {
        File::GuidRef backgroundGuid;
        switch (type)
        {
        case WeaponType::SWORD:
            backgroundGuid = File::GuidRef("5b57821b-eb10-45e4-bb3d-13657c615a41");
            break;
        case WeaponType::WARHAMMER:
            backgroundGuid = File::GuidRef("a0b51873-19db-4a0d-8759-3906b80f7f0b");
            break;
        case WeaponType::DAGGER:
            backgroundGuid = File::GuidRef("04830fb3-56a6-4f27-9f50-0b3500dc2cdb");
            break;
        default:
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Unknown weapon type: " + std::to_string(static_cast<int>(type)));
            backgroundGuid = File::NULL_GUID; // Default to SWORD
            break;
        }
        return backgroundGuid;
    }
};
struct GetIconGuid
{
    enum class IconType : char
    {
        HIT,
        CRITICAL,
        SPEED,
        ATTACK_COUNT,
        BLEEDING,
        STUN
    };
    std::wstring operator()(const IconType type) const
    {
        std::wstring guid;
        switch (type)
        {
        case IconType::HIT:
            guid = L"91f7e4d1-d671-44ed-b552-897fa07c4d86";
            break;
        case IconType::CRITICAL:
            guid = L"59dd5d7d-8612-48e5-a994-ecef25c3e1f2";
            break;
        case IconType::SPEED:
            guid = L"8ab88411-6ba9-41ad-9721-957edbdb27d2";
            break;
        case IconType::ATTACK_COUNT:
            guid = L"4a9abaf8-2732-4d51-9524-cab800f90b75";
            break;
        case IconType::BLEEDING:
            guid = L"f1182a70-6da4-4f1d-9402-17ac2da6237f";
            break;
        case IconType::STUN:
            guid = L"121cad23-2058-4ad2-b7b2-4fd8a1aefddb";
            break;
        }
        return guid;
    }
};

struct GetWeaponDescription
{
    std::pair<std::wstring, std::wstring> operator()(const int weaponId) const
    {
        constexpr GetIconGuid               getIconGuid;
        std::pair<std::wstring, std::wstring> description{};
        switch (weaponId)
        {
        case 1: // 녹슨자의 검
            description.first += L"<Description>"
                                L"<Text color=\"#ffd966\">치명적</Text>"
                                L"<Text color=\"#ffffff\"> 영구적으로</Text>"
                                L"<Image guid=\"" +
                                getIconGuid(GetIconGuid::IconType::CRITICAL) +
                                L"\"/>"
                                L"<Text color=\"#ffffff\">+1</Text>"
                                L"</Description>";
            break;
        case 2: // 돌격 대장의 망치
            description.first += L"<Description>"
                                L"<Text color=\"#ffd966\">무결점</Text>"
                                L"<Text color=\"#ffffff\"> 적</Text>"
                                L"<Image guid=\"" +
                                getIconGuid(GetIconGuid::IconType::STUN) +
                                L"\"/>"
                                L"<Text color=\"#ffffff\">1 부여</Text>"
                                L"</Description>";
            break;
        case 3: // 돌파자의 장검
            description.first  += L"<Description>"
                                 L"<Text color=\"#ffffff\">(체력=100%)</Text>"
                                 L"<Text color=\"#5c6c8b\"> 적</Text>"
                                 L"<Text color=\"#ffffff\"> 대상</Text>"
                                 L"</Description>";
            description.second += L"<Description>"
                                 L"<Image guid=\"" +
                                 getIconGuid(GetIconGuid::IconType::HIT) +
                                 L"\"/>"
                                 L"<Text color=\"#ffffff\">+8</Text>"
                                 L"<Image guid=\"" +
                                 getIconGuid(GetIconGuid::IconType::CRITICAL) +
                                 L"\"/>"
                                 L"<Text color=\"#ffffff\">+10</Text>"
                                 L"</Description>";
            break;
        case 4: // 제물의 단검
            description.first += L"<Description>"
                                L"<Text color=\"#ffd966\">무결점</Text>"
                                L"<Text color=\"#ffffff\"> 적</Text>"
                                L"<Image guid=\"" +
                                getIconGuid(GetIconGuid::IconType::BLEEDING) +
                                L"\"/>"
                                L"<Text color=\"#ffffff\">1 부여</Text>"
                                L"</Description>";
            break;
        default:
            description.first += L"<Description>"
                                L"<Text color=\"#ffffff\">알 수 없는 무기</Text>"
                                L"</Description>";
            break;
        }
        return description;
    }
};

WeaponViewModel::WeaponViewModel(MVVM::Model<TurnActor*>& model): MVVM::ViewModel<TurnActor*, WeaponUIData>(model) {}

WeaponUIData WeaponViewModel::Convert(TurnActor* const& value)
{
    WeaponUIData data{};
    if (nullptr != value && typeid(*value) == typeid(Player))
    {
        data.Enable                     = true;
        WeaponSystem*      weaponSystem = WeaponSystem::GetInstance();
        const WeaponStats& state        = weaponSystem->GetCurrentWeaponStats();
        const int          weaponId     = state.WeaponID;
        const WeaponType   type         = state.Type;
        data.WeaponIcon                 = GetWeaponIcon()(weaponId);
        data.WeaponName                 = state.WeaponName;
        data.HitDamage                  = state.HitDamage;
        data.CriticalDamage             = state.CriticalDamage;
        data.Speed                      = state.Speed;
        data.AttackCount                = state.AttackCount;
        data.Background                 = GetWeaponBackground()(type);
        auto [description1, description2] = GetWeaponDescription()(weaponId);
        data.Description1                 = description1;
        data.Description2                 = description2;
    }
    else
    {
        data.Enable = false;
    }

    return data;
}