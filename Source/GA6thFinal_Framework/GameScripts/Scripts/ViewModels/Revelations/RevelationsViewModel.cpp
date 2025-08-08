#include "pchScripts.h"
#include "RevelationsViewModel.h"

#include "RevelationSystem/RevelationElement/RevelationElement.h"

struct GetRevelationIcon
{
    File::GuidRef operator()(const int revelationID) const
    {
        File::GuidRef iconGuid;
        switch (revelationID)
        {
        case 13010: // 곡예
        {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(111304);
            iconGuid               = path.ToGuid();
        }
        break;
        case 13011: // 부서진 갑주
        {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(111301);
            iconGuid               = path.ToGuid();
        }
        break;
        case 13004: // 붉은 목요일
        {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(111300);
            iconGuid               = path.ToGuid();
        }
        break;
        case 13008: // 산 제물
        {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(111303);
            iconGuid               = path.ToGuid();
        }
        break;
        case 13001: // 취약
        {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(111302);
            iconGuid               = path.ToGuid();
        }
        break;
        default:
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Unknown revelation ID: " + std::to_string(revelationID));
            iconGuid = File::NULL_GUID; // Default to SWORD
            break;
        }
        return iconGuid;
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
        STUN,
        COMBO
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
        case IconType::COMBO: {
            const File::Path& path = UmFileSystem.GetPathFromAssetID(110023);
            guid                   = path.ToGuid();
        }
        break;
        }
        return guid;
    }
};

struct GetRevelationDescription
{
    std::pair<std::wstring, std::wstring> operator()(const int weaponId) const
    {
        constexpr GetIconGuid                 getIconGuid;
        std::pair<std::wstring, std::wstring> description{};
        switch (weaponId)
        {
        case 13010: // 곡예
            description.first += L"<Description>"
                                 L"<Image guid=\"" +
                                 getIconGuid(GetIconGuid::IconType::COMBO) +
                                 L"\"/>"
                                 L"<Text color=\"#ffca00\">x8 달성</Text>"
                                 L"</Description>";
            description.second += L"<Description>"
                                  L"<Text color=\"#9bb9f0\">모든 적</Text>"
                                  L"<Text color=\"#ffffff\"> 공격</Text>"
                                  L"</Description>";
            break;
        case 13011: // 부서진 갑주
            description.first += L"<Description>"
                                 L"<Image guid=\"" +
                                 getIconGuid(GetIconGuid::IconType::COMBO) +
                                 L"\"/>"
                                 L"<Text color=\"#ffca00\">~2 달성</Text>"
                                 L"</Description>";
            description.second += L"<Description>"
                                  L"<Text color=\"#9bb9f0\">자신</Text>"
                                  L"<Text color=\"#a2c989\"> 15 회복</Text>"
                                  L"</Description>";
            break;
        case 13004: // 붉은 목요일
            description.first += L"<Description>"
                                 L"<Image guid=\"" +
                                 getIconGuid(GetIconGuid::IconType::COMBO) +
                                 L"\"/>"
                                 L"<Text color=\"#ffca00\">x4 달성</Text>"
                                 L"</Description>";
            description.second += L"<Description>"
                                  L"<Text color=\"#9bb9f0\">적</Text>"
                                  L"<Image guid=\"" +
                                  getIconGuid(GetIconGuid::IconType::BLEEDING) +
                                  L"\"/>"
                                  L"<Text color=\"#ffffff\"> 3 부여</Text>"
                                  L"</Description>";
            break;
        case 13008: // 산 제물
            description.first += L"<Description>"
                                 L"<Text color=\"#ffca00\">일회성</Text>"
                                 L"</Description>";
            description.second += L"<Description>"
                                  L"<Image guid=\"" +
                                  getIconGuid(GetIconGuid::IconType::BLEEDING) +
                                  L"\"/>"
                                  L"<Text color=\"#9bb9f0\"> 적</Text>"
                                  L"<Text color=\"#ffffff\"> 대상</Text>"
                                  L"<Image guid=\"" +
                                  getIconGuid(GetIconGuid::IconType::CRITICAL) +
                                  L"\"/>"
                                  L"<Text color=\"#ffffff\"> x3</Text>"
                                  L"</Description>";
            break;
        case 13001: // 취약
            description.first += L"<Description>"
                                 L"<Image guid=\"" +
                                 getIconGuid(GetIconGuid::IconType::COMBO) +
                                 L"\"/>"
                                 L"<Text color=\"#ffca00\">4~6 달성</Text>"
                                 L"</Description>";
            description.second += L"<Description>"
                                  L"<Text color=\"#9bb9f0\"> 적</Text>"
                                  L"<Text color=\"#ffffff\"> 대상</Text>"
                                  L"<Image guid=\"" +
                                  getIconGuid(GetIconGuid::IconType::HIT) +
                                  L"\"/>"
                                  L"<Text color=\"#ffffff\"> +4</Text>"
                                  L"<Image guid=\"" +
                                  getIconGuid(GetIconGuid::IconType::CRITICAL) +
                                  L"\"/>"
                                  L"<Text color=\"#ffffff\"> +4</Text>"
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

RevelationsViewModel::RevelationsViewModel(MVVM::Model<std::vector<std::shared_ptr<RevelationElement>>>& model)
    : ViewModel(model)
{
}

std::vector<RevelationUIData> RevelationsViewModel::Convert(
    const std::vector<std::shared_ptr<RevelationElement>>& value)
{
    _uiData.clear();

    for (const auto& revelationElement : value)
    {
        const int revelationID = revelationElement->RevelationID;

        RevelationUIData uiData;

        uiData.Name = revelationElement->ElementName;
        uiData.Icon = GetRevelationIcon()(revelationID);
        auto [description1, description2] = GetRevelationDescription()(revelationID);
        uiData.Description1               = description1;
        uiData.Description2               = description2;

        _uiData.push_back(uiData);
    }

    return _uiData;
}