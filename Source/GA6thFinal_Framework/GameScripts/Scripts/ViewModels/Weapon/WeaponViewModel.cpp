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
        case 11004: // 녹슨자의 검
        {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(111000);
            portraitGuid           = path.ToGuid();
        }
        break;
        case 11200: // 돌격 대장의 망치
        {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(111200);
            portraitGuid           = path.ToGuid();
        }
        break;
        case 11000: // 돌파자의 장검
        {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(111001);
            portraitGuid           = path.ToGuid();
        }
        break;
        case 11101: // 제물의 단검
        {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(111100);
            portraitGuid           = path.ToGuid();
        }
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
        case WeaponType::SWORD: {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(110000);
            backgroundGuid         = path.ToGuid();
        }
        break;
        case WeaponType::DAGGER: {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(110001);
            backgroundGuid         = path.ToGuid();
        }
        break;
        case WeaponType::WARHAMMER: {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(110002);
            backgroundGuid         = path.ToGuid();
        }
        break;
        default:
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Unknown weapon type: " + std::to_string(static_cast<int>(type)));
            backgroundGuid = File::NULL_GUID; // Default to SWORD
            break;
        }
        return backgroundGuid;
    }
};

namespace
{
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
            case IconType::HIT: {
                const File::Path& path = UmFileSystem.GetPathFromAssetID(110010);
                guid                   = path.ToGuid();
            }
            break;
            case IconType::CRITICAL: {
                const File::Path& path = UmFileSystem.GetPathFromAssetID(110011);
                guid                   = path.ToGuid();
            }
            break;
            case IconType::SPEED: {
                const File::Path& path = UmFileSystem.GetPathFromAssetID(110013);
                guid                   = path.ToGuid();
            }
            break;
            case IconType::ATTACK_COUNT: {
                const File::Path& path = UmFileSystem.GetPathFromAssetID(110012);
                guid                   = path.ToGuid();
            }
            break;
            case IconType::BLEEDING: {
                const File::Path& path = UmFileSystem.GetPathFromAssetID(111700);
                guid                   = path.ToGuid();
            }
            break;
            case IconType::STUN: {
                const File::Path& path = UmFileSystem.GetPathFromAssetID(111703);
                guid                   = path.ToGuid();
            }
            break;
            }
            return guid;
        }
    };
    struct GetWeaponDescription
    {
        std::pair<std::wstring, std::wstring> operator()(const int weaponId) const
        {
            constexpr GetIconGuid                 getIconGuid;
            std::pair<std::wstring, std::wstring> description{};
            switch (weaponId)
            {
            case 11004: // 녹슨자의 검
                description.first += L"<Description>"
                                     L"<Text color=\"#ffd966\">치명적</Text>"
                                     L"</Description>";
                description.second += L"<Description>"
                                      L"<Text color=\"#ffffff\">영구적으로</Text>"
                                      L"<Image guid=\"" +
                                      getIconGuid(GetIconGuid::IconType::CRITICAL) +
                                      L"\"/>"
                                      L"<Text color=\"#ffffff\">+1</Text>"
                                      L"</Description>";
                break;
            case 11200: // 돌격 대장의 망치
                description.first += L"<Description>"
                                     L"<Text color=\"#ffd966\">무결점</Text>"
                                     L"</Description>";
                description.second += L"<Description>"
                                      L"<Text color=\"#ffffff\">적</Text>"
                                      L"<Image guid=\"" +
                                      getIconGuid(GetIconGuid::IconType::STUN) +
                                      L"\"/>"
                                      L"<Text color=\"#ffffff\">1 부여</Text>"
                                      L"</Description>";
                break;
            case 11000: // 돌파자의 장검
                description.first += L"<Description>"
                                     L"<Text color=\"#ffffff\">(체력=100%)</Text>"
                                     L"<Text color=\"#5c6c8b\">적</Text>"
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
            case 11101: // 제물의 단검
                description.first += L"<Description>"
                                     L"<Text color=\"#ffd966\">무결점</Text>"
                                     L"</Description>";
                description.second += L"<Description>"
                                      L"<Text color=\"#ffffff\">적</Text>"
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
} // namespace


WeaponViewModel::WeaponViewModel(MVVM::Model<TurnActor*>& model): MVVM::ViewModel<TurnActor*, WeaponUIData>(model) {}

WeaponUIData WeaponViewModel::Convert(TurnActor* const& value)
{
    WeaponUIData data{};
    if (nullptr != value && typeid(*value) == typeid(Player))
    {
        data.Enable                     = true;
        WeaponSystem*      weaponSystem = SingletonComponent<WeaponSystem>::GetInstance();
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